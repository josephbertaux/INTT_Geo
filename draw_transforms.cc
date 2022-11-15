void draw_transforms()
{

	TFile* file = TFile::Open("intt_transforms.root", "READ");
	if(!file){std::cout << "file" << std::endl;return;}
	TTree* tree = (TTree*)file->Get("intt_transforms");
	if(!tree){std::cout << "tree" << std::endl;return;}


	std::vector<std::string> sensors = {"snsr_A", "snsr_B", "snsr_C", "snsr_D"};
	std::map<std::string, std::tuple<float*, float*, float*, Long64_t*, TH1F*>> params =
	{
				//address	//mean		//std. dev.	//counts
//		{"dx_m",	{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
//		{"dy_m",	{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
//		{"dz_m",	{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
//		{"a_m",		{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
//		{"b_m",		{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
//		{"g_m",		{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},

		{"dx_n",	{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
		{"dy_n",	{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
		{"dz_n",	{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
		{"a_n",		{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
		{"b_n",		{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
		{"g_n",		{new float(0),	new float(0), 	new float(0),	new Long64_t(0), nullptr}}

//		{"dx_r",	{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
//		{"dy_r",	{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
//		{"dz_r",	{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
//		{"a_r",		{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
//		{"b_r",		{new float(0),	new float(0),	new float(0),	new Long64_t(0), nullptr}},
//		{"g_r",		{new float(0),	new float(0), 	new float(0),	new Long64_t(0), nullptr}}
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
			if(name.find(*snsr) == std::string::npos)continue;

			for(auto par = params.begin(); par != params.end(); ++par)
			{
				*std::get<1>(par->second) += (*std::get<0>(par->second));
				*std::get<2>(par->second) += (*std::get<0>(par->second)) * (*std::get<0>(par->second));

				*std::get<3>(par->second) += 1;
			}
		}

		std::cout << *snsr << std::endl;
		for(auto par = params.begin(); par != params.end(); ++par)
		{
			float m = (*std::get<1>(par->second)) / (*std::get<3>(par->second));
			float s = (*std::get<2>(par->second)) / (*std::get<3>(par->second));
			s = s - m * m;
			if(s < 0)s *= -1.0;
			s = sqrt(s);
	
			name = *snsr + "_" + par->first;	
			std::get<4>(par->second) = new TH1F(name.c_str(), name.c_str(), 9, m - 4.0 * s, m + 4.0 * s);

			std::cout << "\t" << par->first << std::endl;
			std::cout << "\t\tm:\t" << m << std::endl;
			std::cout << "\t\ts:\t" << s << std::endl;
		}

		for(Long64_t n = 0; n < tree->GetEntriesFast(); ++n)
		{
			tree->GetEntry(n);

			//bad point--OGP machine failed to find one of the crosses
			if(name.find("B1L114") != std::string::npos)continue;
			if(name.find(*snsr) == std::string::npos)continue;

			for(auto par = params.begin(); par != params.end(); ++par)
			{
				std::get<4>(par->second)->Fill(*std::get<0>(par->second));
			}
		}

		for(auto par = params.begin(); par != params.end(); ++par)
		{
			std::get<4>(par->second)->Draw();

			TCanvas* c1 = (TCanvas*)gROOT->FindObject("c1");
			name = "param_plots/";
			name += *snsr + "_" + par->first + ".png";

			if(c1)c1->SaveAs(name.c_str());
		}
	}
}
