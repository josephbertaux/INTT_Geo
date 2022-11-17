void write_transforms()
{
	TFile* transforms_file = TFile::Open("intt_transforms.root", "READ");
	if(!transforms_file){std::cout << "file" << std::endl;return;}
	TTree* transforms_tree = (TTree*)transforms_file->Get("intt_transforms");
	if(!transforms_tree){std::cout << "tree" << std::endl;return;}

	std::string name;
	std::string* name_ptr = &name;
	transforms_tree->SetBranchStatus("name", 1);	transforms_tree->SetBranchAddress("name", &name_ptr);

	float dx_m;	transforms_tree->SetBranchStatus("dx_m", 1);	transforms_tree->SetBranchAddress("dx_m", &dx_m);
	float dy_m;	transforms_tree->SetBranchStatus("dy_m", 1);	transforms_tree->SetBranchAddress("dy_m", &dy_m);
	float dz_m;	transforms_tree->SetBranchStatus("dz_m", 1);	transforms_tree->SetBranchAddress("dz_m", &dz_m);

	std::map<std::string, std::tuple<bool*, float*, float*, float*, float*, float*, float*>> m =
	{
		{"snsr_A", {new bool(false), new float(0.0), new float(0.0), new float(0.0), new float(0.0), new float(0.0), new float(0.0)}},
		{"snsr_B", {new bool(false), new float(0.0), new float(0.0), new float(0.0), new float(0.0), new float(0.0), new float(0.0)}},
		{"snsr_C", {new bool(false), new float(0.0), new float(0.0), new float(0.0), new float(0.0), new float(0.0), new float(0.0)}},
		{"snsr_D", {new bool(false), new float(0.0), new float(0.0), new float(0.0), new float(0.0), new float(0.0), new float(0.0)}},
	};

	for(auto itr = m.begin(); itr != m.end(); ++itr)
	{
		*std::get<1>(itr->second) = 0;
		*std::get<4>(itr->second) = 0;
		*std::get<5>(itr->second) = 0;
		*std::get<6>(itr->second) = 0;
	}

	Long64_t N = 0;
	for(Long64_t n = 0; n < transforms_tree->GetEntriesFast(); ++n)
	{
		transforms_tree->GetEntry(n);

		//bad point--we are ommitting it
		if(name.find("B1L114") != std::string::npos)continue;

		for(auto itr = m.begin(); itr != m.end(); ++itr)
		{
			if(name.find(itr->first) == std::string::npos)continue;

			*std::get<1>(itr->second) += 1.0;
			*std::get<4>(itr->second) += dx_m;
			*std::get<5>(itr->second) += dy_m;
			*std::get<6>(itr->second) += dz_m;
		}
	}
	for(auto itr = m.begin(); itr != m.end(); ++itr)
	{
		*std::get<4>(itr->second) /= *std::get<1>(itr->second);
		*std::get<5>(itr->second) /= *std::get<1>(itr->second);
		*std::get<6>(itr->second) /= *std::get<1>(itr->second);

		std::cout << itr->first << std::endl;
		std::cout << "\taverage x:\t" << *std::get<4>(itr->second) << std::endl;
		std::cout << "\taverage y:\t" << *std::get<5>(itr->second) << std::endl;
		std::cout << "\taverage z:\t" << *std::get<6>(itr->second) << std::endl;
	}

	FILE* file;
	file = fopen("intt_transforms.txt", "w");

	fprintf(file, "%s\t\t%s\t\t%s\t\t%s\n", "Name", "x (mm)", "y (mm)", "z (mm)");
	bool b;
	char buff[16];
	std::string temp;
	int ladder;
	int ladders[4] = {12, 12, 16, 16};
	for(int lyr = 0; lyr < 4; ++lyr)
	{
		for(int ldr = 0; ldr < ladders[lyr]; ++ldr)
		{
			sprintf(buff, "B%dL%03d", lyr / 2, (lyr % 2) * 100 + ldr);
			temp = buff;

			for(auto itr = m.begin(); itr != m.end(); ++itr)
			{
				*std::get<0>(itr->second) = false;
				*std::get<1>(itr->second) = *std::get<4>(itr->second);
				*std::get<2>(itr->second) = *std::get<5>(itr->second);
				*std::get<3>(itr->second) = *std::get<6>(itr->second);
			}

			for(Long64_t n = 0; n < transforms_tree->GetEntriesFast(); ++n)
			{
				transforms_tree->GetEntry(n);

				//bad point--we are ommitting it
				if(name.find("B1L114") != std::string::npos)continue;
				if(name.find(temp) == std::string::npos)continue;

				for(auto itr = m.begin(); itr != m.end(); ++itr)
				{
					if(name.find(itr->first) == std::string::npos)continue;

					*std::get<0>(itr->second) = true;
					*std::get<1>(itr->second) = dx_m;
					*std::get<2>(itr->second) = dy_m;
					*std::get<3>(itr->second) = dz_m;
				}
			}

			b = true;

			for(auto itr = m.begin(); itr != m.end(); ++itr)
			{
				b = b and *std::get<0>(itr->second);
			}
			
			if(b)
			{
				fprintf(file, "\n%s\n", temp.c_str());
				for(auto itr = m.begin(); itr != m.end(); ++itr)
				{
					fprintf(file, "\t%s\t%+11.8f\t%+11.8f\t%+13.8f\n", itr->first.c_str(), *std::get<1>(itr->second), *std::get<2>(itr->second), *std::get<3>(itr->second));
				}
			}
			else
			{
				fprintf(file, "\n%s (Data missing--using mean values)\n", temp.c_str());
				for(auto itr = m.begin(); itr != m.end(); ++itr)
				{
					fprintf(file, "\t%s\t%+11.8f\t%+11.8f\t%+13.8f\n", itr->first.c_str(), *std::get<4>(itr->second), *std::get<5>(itr->second), *std::get<6>(itr->second));
				}
			}

			
		}
	}

	fclose(file);
}
