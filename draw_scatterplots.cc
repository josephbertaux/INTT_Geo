void draw_scatterplots()
{

	TFile* file = TFile::Open("intt_transforms.root", "READ");
	if(!file){std::cout << "file" << std::endl;return;}
	TTree* tree = (TTree*)file->Get("intt_transforms");
	if(!tree){std::cout << "tree" << std::endl;return;}

	std::vector<std::string> sensors = {"snsr_A", "snsr_B", "snsr_C", "snsr_D"};
	std::map<std::string, int> ladders =
	{
		{"B0L0", 0},
		{"B0L1", 12},
		{"B1L0", 24},
		{"B1L1", 40}
	};
	std::map<std::string, std::tuple<float*, float*, float*, Long64_t*, TTree*>> params =
	{
		{"dx_m",	{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
		{"dy_m",	{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
		{"dz_m",	{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
	};

	std::string name;
	std::string* name_ptr = &name;
	tree->SetBranchStatus("name", 1);
	tree->SetBranchAddress("name", &name_ptr);
	for(auto par = params.begin(); par != params.end(); ++par)
	{
		tree->SetBranchStatus(par->first.c_str(), 1);
		tree->SetBranchAddress(par->first.c_str(), std::get<0>(par->second));
	}

	for(auto snsr = sensors.begin(); snsr != sensors.end(); ++snsr)
	{
		for(auto par = params.begin(); par != params.end(); ++par)
		{
			*std::get<1>(par->second) = 0;	//mean
			*std::get<2>(par->second) = 0;	//variance
			*std::get<3>(par->second) = 0;	//counts
		}

		for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n)
		{
			tree->GetEntry(n);

			//bad point--OGP machine failed to find one of the crosses
			if(name.find("B1L114") != std::string::npos)continue;
			//if(name.find(*lddr) == std::string::npos)continue;
			if(name.find(*snsr) == std::string::npos)continue;

			for(auto par = params.begin(); par != params.end(); ++par)
			{
				*std::get<1>(par->second) += (*std::get<0>(par->second));
				*std::get<2>(par->second) += (*std::get<0>(par->second)) * (*std::get<0>(par->second));

				*std::get<3>(par->second) += 1;
			}
		}

		std::cout << "\t" << *snsr << std::endl;
		for(auto par = params.begin(); par != params.end(); ++par)
		{
			float m = (*std::get<1>(par->second)) / (*std::get<3>(par->second));
			float s = (*std::get<2>(par->second)) / (*std::get<3>(par->second));
			s = s - m * m;
			if(s < 0)s *= -1.0;
			s = sqrt(s);
			*std::get<1>(par->second) = m;
			*std::get<2>(par->second) = s;
	
			name = *snsr + "_" + par->first;
			if(std::get<4>(par->second))delete std::get<4>(par->second);
			std::get<4>(par->second) = new TTree(name.c_str(), name.c_str());
			std::get<4>(par->second)->Branch("ladder", std::get<3>(par->second));
			std::get<4>(par->second)->Branch(par->first.c_str(), std::get<0>(par->second));

			std::cout << "\t\t" << par->first << std::endl;
			std::cout << "\t\t\tm:\t" << m << std::endl;
			std::cout << "\t\t\ts:\t" << s << std::endl;
		}

		for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n)
		{
			tree->GetEntry(n);

			//bad point--OGP machine failed to find one of the crosses
			if(name.find("B1L114") != std::string::npos)continue;
			//if(name.find(*lddr) == std::string::npos)continue;
			if(name.find(*snsr) == std::string::npos)continue;

			for(auto par = params.begin(); par != params.end(); ++par)
			{
				*std::get<3>(par->second) = std::stoi(name.substr(4, 2));
				for(auto lddr = ladders.begin(); lddr != ladders.end(); ++lddr)
				{
					if(name.find(lddr->first) != std::string::npos)*std::get<3>(par->second) += lddr->second;
				}
				std::get<4>(par->second)->Fill();
			}
		}

		for(auto par = params.begin(); par != params.end(); ++par)
		{
			name = par->first + ":ladder";
			std::get<4>(par->second)->SetMarkerStyle(20);
			std::get<4>(par->second)->Draw(name.c_str());

			TCanvas* c1 = (TCanvas*)gROOT->FindObject("c1");
			name = "param_scatterplots/";
			name += *snsr + "_" + par->first + ".png";

			if(c1)
			{
				for(const auto&& obj: *c1->GetListOfPrimitives()){std::cout << obj->GetName() << std::endl;}
				c1->SaveAs(name.c_str());
			}
		}
	}
}
