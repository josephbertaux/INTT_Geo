void get_intt_transforms()
{
	TFile* geo_file = TFile::Open("intt_geo.root", "READ");
	if(!geo_file)return;
	TTree* geo_tree = (TTree*)geo_file->Get("intt_geo");
	if(!geo_tree)return;

	TFile* file = TFile::Open("intt_transforms.root", "RECREATE");
	TTree* tree = new TTree("intt_transforms", "intt_transforms");
	tree->SetDirectory(file);

	std::cout << "here" << std::endl;

	std::string name;
	std::string* name_ptr = &name;

	float mx, nx, dx;
	float my, ny, dy;
	float mz, nz, dz;
	float alpha, beta, gamma;

	tree->Branch("name", &name);

	tree->Branch("mx", &mx);
	tree->Branch("nx", &nx);
	tree->Branch("dx", &dx);

	tree->Branch("my", &my);
	tree->Branch("ny", &ny);
	tree->Branch("dy", &dy);

	tree->Branch("mz", &mz);
	tree->Branch("nz", &nz);
	tree->Branch("dz", &dz);

	tree->Branch("alpha", &alpha);
	tree->Branch("beta", &beta);
	tree->Branch("gamma", &gamma);

	geo_tree->SetBranchStatus("*", 0);

	geo_tree->SetBranchStatus("name", 1);
	geo_tree->SetBranchAddress("name", &name_ptr);

	geo_tree->SetBranchStatus("mx", 1);
	geo_tree->SetBranchAddress("mx", &mx);
	geo_tree->SetBranchStatus("nx", 1);
	geo_tree->SetBranchAddress("nx", &nx);

	geo_tree->SetBranchStatus("my", 1);
	geo_tree->SetBranchAddress("my", &my);
	geo_tree->SetBranchStatus("ny", 1);
	geo_tree->SetBranchAddress("ny", &ny);

	geo_tree->SetBranchStatus("mz", 1);
	geo_tree->SetBranchAddress("mz", &mz);
	geo_tree->SetBranchStatus("nz", 1);
	geo_tree->SetBranchAddress("nz", &nz);

	std::vector<std::string> component_names = {"snsr_A", "snsr_B", "snsr_C", "snsr_D"};

	bool b1, b2, b3, b4;
	TVector3 u1, v1;
	TVector3 u2, v2;
	TVector3 u3, v3;
	TVector3 u4, v4;

	std::map<std::string, std::tuple<bool*, TVector3*, TVector3*>> lddr_map =
	{
		{"ndcp_1", {&b1, &u1, &v1}},
		{"ndcp_2", {&b2, &u2, &v2}},
		{"ndcp_3", {&b3, &u3, &v3}},
		{"ndcp_4", {&b4, &u4, &v4}}
	};

	std::map<std::string, std::tuple<bool*, TVector3*, TVector3*>> snsr_map =
	{
		{"crss_1", {&b1, &u1, &v1}},
		{"crss_2", {&b2, &u2, &v2}},
		{"crss_3", {&b3, &u3, &v3}},
		{"crss_4", {&b4, &u4, &v4}}
	};

	TVector3 lddr_c;
	TVector3 lddr_x;
	TVector3 lddr_y;
	TVector3 lddr_z;
	TMatrix lddr_transform(4, 4); //from ladder to global

	TVector3 snsr_c;
	TVector3 snsr_x;
	TVector3 snsr_y;
	TVector3 snsr_z;
	TMatrix snsr_transform(4, 4); //from sensor to global
	TMatrix m(4, 1);
	TMatrix n(4, 1);

	TMatrix T(4, 4);
	TMatrix t(4, 1);

	int lyr;
	int ladders[4] = {12, 12, 16, 16};
	int ldr;
	int i;
	Long64_t N;
	char buff[16];
	std::string temp;
	for(lyr = 0; lyr < 4; ++lyr)
	{
		for(ldr = 0; ldr < ladders[lyr]; ++ldr)
		{
			sprintf(buff, "B%iL%03i", lyr / 2, (lyr % 2) * 100 + ldr);
			temp = buff;

			b1 = true;
			b2 = true;
			b3 = true;
			b4 = true;

			//Search through the tree and get the ladder's affine transform
			for(N = 0; N < geo_tree->GetEntriesFast(); ++N)
			{
				geo_tree->GetEntry(N);
				if(name.find(temp) == std::string::npos)continue;

				for(auto itr = lddr_map.begin(); itr != lddr_map.end(); ++itr)
				{
					if(name.find(itr->first) == std::string::npos)continue;

					*std::get<0>(itr->second) = false;

					std::get<1>(itr->second)->SetX(mx);
					std::get<1>(itr->second)->SetY(my);
					std::get<1>(itr->second)->SetZ(mz);

					std::get<2>(itr->second)->SetX(nx);
					std::get<2>(itr->second)->SetY(ny);
					std::get<2>(itr->second)->SetZ(nz);
				}
			}

			if(b1 or b2 or b3 or b4)continue;

			mx =	( ((u4.Y() - u2.Y()) / (u4.X() - u2.X()) * u2.X() - u2.Y()) - ((u3.Y() - u1.Y()) / (u3.X() - u1.X()) * u1.X() - u1.Y()) ) /
				( (u4.Y() - u2.Y()) / (u4.X() - u2.X()) - (u3.Y() - u1.Y()) / (u3.X() - u1.X()) );
			my =	( ((u4.X() - u2.X()) / (u4.Y() - u2.Y()) * u2.Y() - u2.X()) - ((u3.X() - u1.X()) / (u3.Y() - u1.Y()) * u1.Y() - u1.X()) ) /
				( (u4.X() - u2.X()) / (u4.Y() - u2.Y()) - (u3.X() - u1.X()) / (u3.Y() - u1.Y()) );
			mz =	(u1.Z() + u2.Z() + u3.Z() + u4.Z()) * 0.25;

			nx =	( ((v4.Y() - v2.Y()) / (v4.X() - v2.X()) * v2.X() - v2.Y()) - ((v3.Y() - v1.Y()) / (v3.X() - v1.X()) * v1.X() - v1.Y()) ) /
				( (v4.Y() - v2.Y()) / (v4.X() - v2.X()) - (v3.Y() - v1.Y()) / (v3.X() - v1.X()) );
			ny =	( ((v4.X() - v2.X()) / (v4.Y() - v2.Y()) * v2.Y() - v2.X()) - ((v3.X() - v1.X()) / (v3.Y() - v1.Y()) * v1.Y() - v1.X()) ) /
				( (v4.X() - v2.X()) / (v4.Y() - v2.Y()) - (v3.X() - v1.X()) / (v3.Y() - v1.Y()) );
			nz =	(v1.Z() + v2.Z() + v3.Z() + v4.Z()) * 0.25;

			lddr_c.SetX(mx - nx);
			lddr_c.SetY(my - ny);
			lddr_c.SetZ(mz - nz);
			lddr_x = ((u3 - u1).Unit() - (u4 - u2).Unit()).Unit();
			lddr_y = ((u3 - u1).Unit() + (u4 - u2).Unit()).Unit();
			lddr_z = lddr_x.Cross(lddr_y).Unit();

			lddr_transform[0][0] = lddr_x.X();	lddr_transform[0][1] = lddr_y.X();	lddr_transform[0][2] = lddr_z.X();	lddr_transform[0][3] = lddr_c.X();
			lddr_transform[1][0] = lddr_x.Y();	lddr_transform[1][1] = lddr_y.Y();	lddr_transform[1][2] = lddr_z.Y();	lddr_transform[1][3] = lddr_c.Y();
			lddr_transform[2][0] = lddr_x.Z();	lddr_transform[2][1] = lddr_y.Z();	lddr_transform[2][2] = lddr_z.Z();	lddr_transform[2][3] = lddr_c.Z();
			lddr_transform[3][0] = 0.0;		lddr_transform[3][1] = 0.0;		lddr_transform[3][2] = 0.0;		lddr_transform[3][3] = 1.0;

			std::cout << std::endl;
			std::cout << lddr_transform[0][0] << "\t" << lddr_transform[0][1] << "\t" << lddr_transform[0][2] << "\t" << lddr_transform[0][3] << "\t" << std::endl;
			std::cout << lddr_transform[1][0] << "\t" << lddr_transform[1][1] << "\t" << lddr_transform[1][2] << "\t" << lddr_transform[1][3] << "\t" << std::endl;
			std::cout << lddr_transform[2][0] << "\t" << lddr_transform[2][1] << "\t" << lddr_transform[2][2] << "\t" << lddr_transform[2][3] << "\t" << std::endl;
			std::cout << lddr_transform[3][0] << "\t" << lddr_transform[3][1] << "\t" << lddr_transform[3][2] << "\t" << lddr_transform[3][3] << "\t" << std::endl;
			std::cout << std::endl;

			for(i= 0; i < component_names.size(); ++i)
			{
				b1 = true;
				b2 = true;
				b3 = true;
				b4 = true;

				for(N = 0; N < geo_tree->GetEntriesFast(); ++N)
				{
					geo_tree->GetEntry(N);
					if(name.find(temp) == std::string::npos)continue;
					if(name.find(component_names[i]) == std::string::npos)continue;

					for(auto itr = snsr_map.begin(); itr != snsr_map.end(); ++itr)
					{
						if(name.find(itr->first) == std::string::npos)continue;

						*std::get<0>(itr->second) = false;

						std::get<1>(itr->second)->SetX(mx);
						std::get<1>(itr->second)->SetY(my);
						std::get<1>(itr->second)->SetZ(mz);

						std::get<2>(itr->second)->SetX(nx);
						std::get<2>(itr->second)->SetY(ny);
						std::get<2>(itr->second)->SetZ(nz);
					}
				}

				if(b1 or b2 or b3 or b4)continue;

				mx =	( ((u4.Y() - u2.Y()) / (u4.X() - u2.X()) * u2.X() - u2.Y()) - ((u3.Y() - u1.Y()) / (u3.X() - u1.X()) * u1.X() - u1.Y()) ) /
					( (u4.Y() - u2.Y()) / (u4.X() - u2.X()) - (u3.Y() - u1.Y()) / (u3.X() - u1.X()) );
				my =	( ((u4.X() - u2.X()) / (u4.Y() - u2.Y()) * u2.Y() - u2.X()) - ((u3.X() - u1.X()) / (u3.Y() - u1.Y()) * u1.Y() - u1.X()) ) /
					( (u4.X() - u2.X()) / (u4.Y() - u2.Y()) - (u3.X() - u1.X()) / (u3.Y() - u1.Y()) );
				mz =	(u1.Z() + u2.Z() + u3.Z() + u4.Z()) * 0.25;

				nx =	( ((v4.Y() - v2.Y()) / (v4.X() - v2.X()) * v2.X() - v2.Y()) - ((v3.Y() - v1.Y()) / (v3.X() - v1.X()) * v1.X() - v1.Y()) ) /
					( (v4.Y() - v2.Y()) / (v4.X() - v2.X()) - (v3.Y() - v1.Y()) / (v3.X() - v1.X()) );
				ny =	( ((v4.X() - v2.X()) / (v4.Y() - v2.Y()) * v2.Y() - v2.X()) - ((v3.X() - v1.X()) / (v3.Y() - v1.Y()) * v1.Y() - v1.X()) ) /
					( (v4.X() - v2.X()) / (v4.Y() - v2.Y()) - (v3.X() - v1.X()) / (v3.Y() - v1.Y()) );
				nz =	(v1.Z() + v2.Z() + v3.Z() + v4.Z()) * 0.25;

				m[0][0] = mx;
				m[1][0] = my;
				m[2][0] = mz;
				m[3][0] = 1.0;

				n[0][0] = nx;
				n[1][0] = ny;
				n[2][0] = nz;
				n[3][0] = 1.0;

				T = lddr_transform; //local to global

				//n is nominal in ladder's frame
				//transform n to global
				t.Mult(T, n);
				n = t;

				//compare with m in global
				t = m - n;

				//now do the sensor's tranform matrix
				//local to global, why we transformed n and not m
				snsr_c.SetX(t[0][0]);
				snsr_c.SetY(t[1][0]);
				snsr_c.SetZ(t[2][0]);
				snsr_x = ((u3 - u1).Unit() - (u4 - u2).Unit()).Unit();
				snsr_y = ((u3 - u1).Unit() + (u4 - u2).Unit()).Unit();
				snsr_z = snsr_x.Cross(snsr_y).Unit();
	
				//local to global
				snsr_transform[0][0] = snsr_x.X();	snsr_transform[0][1] = snsr_y.X();	snsr_transform[0][2] = snsr_z.X();	snsr_transform[0][3] = snsr_c.X();
				snsr_transform[1][0] = snsr_x.Y();	snsr_transform[1][1] = snsr_y.Y();	snsr_transform[1][2] = snsr_z.Y();	snsr_transform[1][3] = snsr_c.Y();
				snsr_transform[2][0] = snsr_x.Z();	snsr_transform[2][1] = snsr_y.Z();	snsr_transform[2][2] = snsr_z.Z();	snsr_transform[2][3] = snsr_c.Z();
				snsr_transform[3][0] = 0.0;		snsr_transform[3][1] = 0.0;		snsr_transform[3][2] = 0.0;		snsr_transform[3][3] = 1.0;

				//(local to global)^-1 * local to global -> ladder's fame to global to snsr frame
				T.Mult(snsr_transform.Invert(), lddr_transform);
				//T is ladder to sensor

				//assign values and fill
				name = temp + "_" + component_names[i];

				dx = T[0][3];
				dy = T[1][3];
				dz = T[2][3];

				alpha = atan(-T[1][2] / T[2][2]); //Something with T
				beta = atan(T[0][2] / sqrt(1 - T[0][2] * T[0][2])); //Something with T
				gamma = atan(-T[0][1] / T[0][0]); //Something with T

				tree->Fill();
			}
		}
	}

	tree->Write();
	file->Write();
	file->Close();
}
