int intt_survey_file(std::string, TTree*);

int intt_survey_data()
{
	char buff[128];
	int ladders[4] = {12, 12, 16, 16};

	TFile* file = TFile::Open("intt_survey_data.root", "RECREATE");
	TTree* tree = new TTree("intt_survey_data", "intt_survey_data");
	tree->SetDirectory(file);

	std::string name;
	float x, nx, dx;
	float y, ny, dy;
	float z, nz, dz;

	tree->Branch("name", &name);

	tree->Branch("x", &x);
	tree->Branch("nx", &nx);
	tree->Branch("dx", &dx);

	tree->Branch("y", &y);
	tree->Branch("ny", &ny);
	tree->Branch("dy", &dy);

	tree->Branch("z", &z);
	tree->Branch("nz", &nz);
	tree->Branch("dz", &dz);

	for(int lyr = 0; lyr < 4; lyr++)
	{
		for(int ldr = 0; ldr < ladders[lyr]; ldr++)
		{
			sprintf(buff, "intt_survey_data/B%iL%03i.txt", lyr / 2, (lyr % 2) * 100 + ldr);
			read_survey_file(buff, tree);
		}
	}

	tree->Write();
	file->Write();
	file->Close();

	return 0;
}

//specifically a survey file
int read_survey_file(std::string file_name, TTree* tree)
{
	std::stringstream output_str;
	int return_val = 0;
	output_str << "read_survery_file(std::string file_name, TTree* tree):" << std::endl;

	std::ifstream file;
	std::string line_str;
	int line_num;

	bool b;
	std::string* name = new std::string;
	float x, nx, dx;
	float y, ny, dy;
	float z, nz, dz;

	std::map<std::string, std::string>::iterator snsr_itr;
	std::map<std::string, std::string> snsr_map;
	snsr_map["Sensor A"] = "snsr_A";
	snsr_map["Sensor B"] = "snsr_B";
	snsr_map["Sensor C"] = "snsr_C";
	snsr_map["Sensor D"] = "snsr_D";

	std::map<std::string, std::string>::iterator crss_itr;
	std::map<std::string, std::string> crss_map;
	crss_map["Cross 1"] = "crss_1";
	crss_map["Cross 2"] = "crss_2";
	crss_map["Cross 3"] = "crss_3";
	crss_map["Cross 4"] = "crss_4";

	std::map<std::string, std::string>::iterator ndcp_itr;
	std::map<std::string, std::string> ndcp_map;
	ndcp_map["Endcap Hole 1"] = "ndcp_1";
	ndcp_map["Endcap Hole 2"] = "ndcp_2";
	ndcp_map["Endcap Hole 3"] = "ndcp_3";
	ndcp_map["Endcap Hole 4"] = "ndcp_4";

	if(!tree)
	{
		output_str << "\tParameter 'tree' is null" << std::endl;
		return_val = 1;
		goto label;
	}

	tree->ResetBranchAddresses();
	
	tree->SetBranchStatus("name", 1);	tree->SetBranchAddress("name", &name);

	tree->SetBranchStatus("x", 1);		tree->SetBranchAddress("x", &x);
	tree->SetBranchStatus("nx", 1);		tree->SetBranchAddress("nx", &nx);
	tree->SetBranchStatus("dx", 1);		tree->SetBranchAddress("dx", &dx);

	tree->SetBranchStatus("y", 1);		tree->SetBranchAddress("y", &y);
	tree->SetBranchStatus("ny", 1);		tree->SetBranchAddress("ny", &ny);
	tree->SetBranchStatus("dy", 1);		tree->SetBranchAddress("dy", &dy);

	tree->SetBranchStatus("z", 1);		tree->SetBranchAddress("z", &z);
	tree->SetBranchStatus("nz", 1);		tree->SetBranchAddress("nz", &nz);
	tree->SetBranchStatus("dz", 1);		tree->SetBranchAddress("dz", &dz);

	file.open(file_name, std::ios_base::in);
	if(!file.is_open())
	{
		output_str << "\tCouldn't open file: '" << file_name << "'" << std::endl;
		return_val = 1;
		goto label;
	}

	line_num = 1;
	for(line_str; std::getline(file, line_str);)
	{
		*name = "";
		b = false;
		for(snsr_itr = snsr_map.begin(); snsr_itr != snsr_map.end(); snsr_itr++)
		{
			if(line_str.find(snsr_itr->first) != std::string::npos)
			{
				b = true;
				break;
			}
		}
		for(crss_itr = crss_map.begin(); crss_itr != crss_map.end(); crss_itr++)
		{
			if(line_str.find(crss_itr->first) != std::string::npos)
			{
				b = true;
				break;
			}
		}

		if(b)
		{
			*name = file_name.substr(file_name.find(".txt") - 6, 6);
			*name += "_";
			*name += snsr_itr->second + "_" + crss_itr->second;

			std::getline(file, line_str);
			line_num++;
			sscanf(line_str.c_str(), "%*s %*s %*s %f %f %f", &nx, &x, &dx);

			std::getline(file, line_str);
			line_num++;
			sscanf(line_str.c_str(), "%*s %*s %*s %f %f %f", &ny, &y, &dy);

			std::getline(file, line_str);
			line_num++;
			sscanf(line_str.c_str(), "%*s %*s %*s %f %f %f", &nz, &z, &dz);
		}

		for(ndcp_itr = ndcp_map.begin(); ndcp_itr != ndcp_map.end(); ndcp_itr++)
		{
			if(line_str.find(ndcp_itr->first) != std::string::npos)
			{
				b = true;
				break;
			}
		}

		if(b and *name == "")
		{
			*name = file_name.substr(file_name.find(".txt") - 6, 6);
			*name += "_";
			*name += ndcp_itr->second;

			std::getline(file, line_str);
			line_num++;
			sscanf(line_str.c_str(), "%*s %*s %*s %f", &x); nx = 0.0; dx = 0.0;

			std::getline(file, line_str);
			line_num++;
			sscanf(line_str.c_str(), "%*s %*s %*s %f", &y); ny = 0.0; dy = 0.0;

			std::getline(file, line_str);
			line_num++;
			sscanf(line_str.c_str(), "%*s %*s %*s %f", &z); nz = 0.0; dz = 0.0;
		}
		
		if(b)
		{
			tree->Fill();

			std::cout << *name << std::endl;
			printf("\tnx: %+010.5f\t x: %+010.5f\tdx: %+010.5f\n", nx, x, dx);
			printf("\tny: %+010.5f\t y: %+010.5f\tdy: %+010.5f\n", ny, y, dy);
			printf("\tnz: %+010.5f\t z: %+010.5f\tdz: %+010.5f\n", nz, z, dz);
		}

		line_num++;
	}

	label:
	delete name;
	file.close();
	output_str << std::ends;
	if(return_val)std::cout << output_str.str() << std::endl;
	return return_val;
}
