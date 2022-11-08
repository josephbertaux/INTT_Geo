void draw_transform_params()
{

	TFile* file = TFile::Open("intt_transforms.root", "READ");
	if(!file){std::cout << "file" << std::endl;return;}
	TTree* tree = (TTree*)file->Get("intt_transforms");
	if(!tree){std::cout << "tree" << std::endl;return;}

	std::string name;
	std::string* name_ptr = &name;
	tree->SetBranchStatus("name", 1);
	tree->SetBranchAddress("name", &name_ptr);

	float dx, dy, dz, a, b, g;
	std::vector<std::string> sensors = {"snsr_A", "snsr_B", "snsr_C", "snsr_D"};
	std::map<std::string, float*> params =
	{
		{"dx",		&dx},
		{"dy",		&dy},
		{"dz",		&dz},
		{"alpha",	&a},
		{"beta",	&b},
		{"gamma",	&g}
	};

	std::string temp;

	for(auto par = params.begin(); par != params.end(); ++par)
	{
		float param_m = 0.0;
		float param_v = 0.0;

		tree->SetBranchStatus(par->first.c_str(), 1);
		tree->SetBranchAddress(par->first.c_str(), par->second);

		Long64_t N = 0;
		for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n)
		{
			tree->GetEntry(n);

			//bad point--OGP machine failed
			if(name.find("B1L114") != std::string::npos)continue;

			param_m += *par->second;
			param_v += (*par->second) * (*par->second);

			++N;
		}

		param_m /= N;
		param_v /= N;
		param_v = sqrt(param_v - param_m * param_m);
	
		std::cout << std::endl;	
		std::cout << par->first << ":" << std::endl;
		std::cout << "\t" << param_m << std::endl;
		std::cout << "\t" << param_v << std::endl;

		TCanvas* c1; 
		TH1F* h = new TH1F("h", par->first.c_str(), 8, param_m - 4.0 * param_v, param_m + 4.0 * param_v);

		for(int j = 0; j < sensors.size(); ++j)
		{
			h->Reset();

			param_m = 0;
			param_v = 0;

			N = 0;
			for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n)
			{
				tree->GetEntry(n);
	
				//bad point--OGP machine failed
				if(name.find("B1L114") != std::string::npos)continue;
				if(name.find(sensors[j]) == std::string::npos)continue;

				h->Fill(*par->second);	
				param_m += *par->second;
				param_v += (*par->second) * (*par->second);
	
				++N;
			}

			param_m /= N;
			param_v /= N;
			param_v = sqrt(param_v - param_m * param_m);

			std::cout << std::endl;	
			std::cout << "\t" << sensors[j] << ":" << std::endl;
			std::cout << "\t\t" << param_m << std::endl;
			std::cout << "\t\t" << param_v << std::endl;

			h->Draw();

			c1 = (TCanvas*)gROOT->FindObject("c1");
			temp = "param_plots/";
			temp += par->first + "_" + sensors[j] + ".png";
			if(c1)c1->SaveAs(temp.c_str());
		}

		h->Reset();

		for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n)
		{
			tree->GetEntry(n);

			//bad point--OGP machine failed
			if(name.find("B1L114") != std::string::npos)continue;

			h->Fill(*par->second);	
		}

		h->Draw();
		c1 = (TCanvas*)gROOT->FindObject("c1");
		temp = "param_plots/";
		temp += par->first + ".png";
		if(c1)c1->SaveAs(temp.c_str());

		delete h;
	}

}
