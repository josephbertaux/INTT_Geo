void compare_survey_data()
{
	TFile* ogp_file = TFile::Open("intt_survey_data.root", "READ");
	if(!ogp_file)return;
	TTree* ogp_tree = (TTree*)ogp_file->Get("intt_survey_data");
	if(!ogp_tree)return;

	std::cout << "running..." << std::endl;
	
	TFile* file = TFile::Open("intt_geo.root", "RECREATE");
	file->cd();
	TTree* tree = new TTree("intt_geo", "intt_geo");
	tree->SetDirectory(file);

	std::string name;
	std::string* name_ptr = &name;
	float mx, nx, dx;
	float my, ny, dy;
	float mz, nz, dz;

	tree->Branch("name", &name);

	tree->Branch("mx", &mx);	//measured x
	tree->Branch("nx", &nx);	//nominal x
	tree->Branch("dx", &dx);	//difference, measured - nominal

	tree->Branch("my", &my);	//measured y
	tree->Branch("ny", &ny);	//nominal y
	tree->Branch("dy", &dy);	//difference, measured - nominal

	tree->Branch("mz", &mz);	//measured z
	tree->Branch("nz", &nz);	//nominal z
	tree->Branch("dz", &dz);	//difference, measured - nominal

	ogp_tree->SetBranchStatus("*", 0);

	ogp_tree->SetBranchStatus("name", 1);
	ogp_tree->SetBranchAddress("name", &name_ptr);

	ogp_tree->SetBranchStatus("x", 1);
	ogp_tree->SetBranchAddress("x", &mx);

	ogp_tree->SetBranchStatus("y", 1);
	ogp_tree->SetBranchAddress("y", &my);

	ogp_tree->SetBranchStatus("z", 1);
	ogp_tree->SetBranchAddress("z", &mz);

	std::map<std::string, std::tuple<float, float, float>>::iterator itr;
	std::map<std::string, std::tuple<float, float, float>> name_map;

	name_map["snsr_A_crss_1"] = std::tuple<float, float, float>{13.50, 6.00, 0.0};
	name_map["snsr_A_crss_2"] = std::tuple<float, float, float>{115.0, 6.00, 0.0};
	name_map["snsr_A_crss_3"] = std::tuple<float, float, float>{115.0, 28.0, 0.0};
	name_map["snsr_A_crss_4"] = std::tuple<float, float, float>{13.50, 28.0, 0.0};

	name_map["snsr_B_crss_1"] = std::tuple<float, float, float>{115.7, 6.00, 0.0};
	name_map["snsr_B_crss_2"] = std::tuple<float, float, float>{245.2, 6.00, 0.0};
	name_map["snsr_B_crss_3"] = std::tuple<float, float, float>{245.2, 28.0, 0.0};
	name_map["snsr_B_crss_4"] = std::tuple<float, float, float>{115.7, 28.0, 0.0};

	name_map["snsr_C_crss_1"] = std::tuple<float, float, float>{246.8, 6.00, 0.0};
	name_map["snsr_C_crss_2"] = std::tuple<float, float, float>{376.3, 6.00, 0.0};
	name_map["snsr_C_crss_3"] = std::tuple<float, float, float>{376.3, 28.0, 0.0};
	name_map["snsr_C_crss_4"] = std::tuple<float, float, float>{246.8, 28.0, 0.0};

	name_map["snsr_D_crss_1"] = std::tuple<float, float, float>{377.0, 6.00, 0.0};
	name_map["snsr_D_crss_2"] = std::tuple<float, float, float>{478.5, 6.00, 0.0};
	name_map["snsr_D_crss_3"] = std::tuple<float, float, float>{478.5, 28.0, 0.0};
	name_map["snsr_D_crss_4"] = std::tuple<float, float, float>{377.0, 28.0, 0.0};

	name_map["ndcp_1"] = std::tuple<float, float, float>{0.000, 0.000, 0.0};
	name_map["ndcp_2"] = std::tuple<float, float, float>{492.0, 0.000, 0.0};
	name_map["ndcp_3"] = std::tuple<float, float, float>{492.0, 34.00, 0.0};
	name_map["ndcp_4"] = std::tuple<float, float, float>{0.000, 34.00, 0.0};

	size_t pos;
	Long64_t n;
	for(n = 0; n < ogp_tree->GetEntriesFast(); ++n)
	{
		ogp_tree->GetEntry(n);

		for(itr = name_map.begin(); itr != name_map.end(); ++itr)
		{
			if(name.find(itr->first) != std::string::npos)
			{
				nx = std::get<0>(itr->second);
				ny = std::get<1>(itr->second);
				nz = std::get<2>(itr->second);

				dx = mx - nx;
				dy = my - ny;
				dz = mz - nz;

				tree->Fill();
				break;
			}
		}
	}

	tree->Write();
	file->Write();
	file->Close();
}
