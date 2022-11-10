TTree* intt_geo_tree;

std::string name;
std::string* name_ptr = &name;

float mx;
float my;
float mz;

float nx;
float ny;
float nz;

float dx_m;
float dy_m;
float dz_m;
float a_m;
float b_m;
float g_m;

float dx_n;
float dy_n;
float dz_n;
float a_n;
float b_n;
float g_n;

float dx_r;
float dy_r;
float dz_r;
float a_r;
float b_r;
float g_r;

bool GetLadderCorners(std::string,
			TVector3*, TVector3*, TVector3*, TVector3*,
			TVector3*, TVector3*, TVector3*, TVector3*);

bool GetSensorCorners(std::string, std::string,
			TVector3*, TVector3*, TVector3*, TVector3*,
			TVector3*, TVector3*, TVector3*, TVector3*);

TMatrix GetTransform(TVector3, TVector3, TVector3, TVector3);

void SetTransformParams(TMatrix, float*, float*, float*, float*, float*, float*);

void PrintTransform(TMatrix, int);

void intt_transforms()
{
	TFile* intt_geo_root = TFile::Open("intt_geo.root", "READ");
	if(!intt_geo_root){std::cout << "intt_geo_root" << std::endl;return;}

	intt_geo_tree = (TTree*)intt_geo_root->Get("intt_geo");
	if(!intt_geo_tree){std::cout << "intt_geo_tree" << std::endl;return;}

	intt_geo_tree->SetBranchStatus("name", 1);	intt_geo_tree->SetBranchAddress("name", &name_ptr);

	intt_geo_tree->SetBranchStatus("mx", 1);	intt_geo_tree->SetBranchAddress("mx", &mx);
	intt_geo_tree->SetBranchStatus("my", 1);	intt_geo_tree->SetBranchAddress("my", &my);
	intt_geo_tree->SetBranchStatus("mz", 1);	intt_geo_tree->SetBranchAddress("mz", &mz);

	intt_geo_tree->SetBranchStatus("nx", 1);	intt_geo_tree->SetBranchAddress("nx", &nx);
	intt_geo_tree->SetBranchStatus("ny", 1);	intt_geo_tree->SetBranchAddress("ny", &ny);
	intt_geo_tree->SetBranchStatus("nz", 1);	intt_geo_tree->SetBranchAddress("nz", &nz);

	TFile* file = TFile::Open("intt_transforms.root", "RECREATE");
	TTree* tree = new TTree("intt_transforms", "intt_transforms");
	tree->SetDirectory(file);

	tree->Branch("name", &name);

	tree->Branch("dx_m", &dx_m);
	tree->Branch("dy_m", &dy_m);
	tree->Branch("dz_m", &dz_m);
	tree->Branch("a_m", &a_m);
	tree->Branch("b_m", &b_m);
	tree->Branch("g_m", &g_m);

	tree->Branch("dx_n", &dx_n);
	tree->Branch("dy_n", &dy_n);
	tree->Branch("dz_n", &dz_n);
	tree->Branch("a_n", &a_n);
	tree->Branch("b_n", &b_n);
	tree->Branch("g_n", &g_n);

	tree->Branch("dx_r", &dx_r);
	tree->Branch("dy_r", &dy_r);
	tree->Branch("dz_r", &dz_r);
	tree->Branch("a_r", &a_r);
	tree->Branch("b_r", &b_r);
	tree->Branch("g_r", &g_r);

	std::string temp;
	char buff[128];

	TVector3 u1, u2, u3, u4;
	TVector3 v1, v2, v3, v4;

	TMatrix ladder_transform_m(4, 4);
	TMatrix ladder_transform_n(4, 4);

	TMatrix sensor_transform_m(4, 4);
	TMatrix sensor_transform_n(4, 4);

	TMatrix A(4, 4);
	TMatrix B(4, 4);
	TMatrix C(4, 4);

	std::vector<std::string> sensor_names = {"snsr_A", "snsr_B", "snsr_C", "snsr_D"};

	int ladders[4] = {12, 12, 16, 16};
	for(int layer = 0; layer < 4; ++layer)
	{
		for(int ladder = 0; ladder < ladders[layer]; ++ladder)
		{
			sprintf(buff, "B%dL%03d", layer / 2, 100 * (layer % 2) + ladder);
			temp = buff;

			if(GetLadderCorners(temp, &u1, &u2, &u3, &u4, &v1, &v2, &v3, &v4))continue;

			ladder_transform_m = GetTransform(u1, u2, u3, u4);
			ladder_transform_n = GetTransform(v1, v2, v3, v4);

			for(auto itr = sensor_names.begin(); itr != sensor_names.end(); ++itr)
			{
				if(GetSensorCorners(temp, *itr, &u1, &u2, &u3, &u4, &v1, &v2, &v3, &v4))continue;

				sensor_transform_m = GetTransform(u1, u2, u3, u4);
				sensor_transform_n = GetTransform(v1, v2, v3, v4);

				A.Mult(sensor_transform_m.Invert(), ladder_transform_m);
				A.Invert(); //sensor to ladder, as measured

				B.Mult(sensor_transform_n.Invert(), ladder_transform_n);
				C.Mult(A, B); //measured sensor to ladder, nominal ladder to sensor
				B.Invert(); //sensor to ladder, nominally

				//std::cout << temp << "_" << *itr << std::endl;
				//PrintTransform(A, 1);

				SetTransformParams(A, &dx_m, &dy_m, &dz_m, &a_m, &b_m, &g_m);
				SetTransformParams(B, &dx_n, &dy_n, &dz_n, &a_n, &b_n, &g_n);
				SetTransformParams(C, &dx_r, &dy_r, &dz_r, &a_r, &b_r, &g_r);

				name = temp + "_" + *itr;
				tree->Fill();
			}
		}
	}

	tree->Write();
	file->Write();
	file->Close();

	intt_geo_root->Close();
}

bool GetLadderCorners(std::string ladder_name,
			TVector3* u1, TVector3* u2, TVector3* u3, TVector3* u4,
			TVector3* v1, TVector3* v2, TVector3* v3, TVector3* v4)
{
	bool b1 = true;
	bool b2 = true;
	bool b3 = true;
	bool b4 = true;

	std::map<std::string, std::tuple<bool*, TVector3*, TVector3*>> m = 
	{
		{"ndcp_1", {&b1, u1, v1}},
		{"ndcp_2", {&b2, u2, v2}},
		{"ndcp_3", {&b3, u3, v3}},
		{"ndcp_4", {&b4, u4, v4}}
	};

	for(Long64_t n = 0; n < intt_geo_tree->GetEntriesFast(); ++n)
	{
		intt_geo_tree->GetEntry(n);
		if(name.find(ladder_name) == std::string::npos)continue;

		for(auto itr = m.begin(); itr != m.end(); ++itr)
		{
			if(name.find(itr->first) != std::string::npos)
			{
				*std::get<0>(itr->second) = false;

				std::get<1>(itr->second)->SetX(mx);
				std::get<1>(itr->second)->SetY(my);
				std::get<1>(itr->second)->SetZ(mz);

				std::get<2>(itr->second)->SetX(nx);
				std::get<2>(itr->second)->SetY(ny);
				std::get<2>(itr->second)->SetZ(nz);
			}
		}
	}

	return b1 or b2 or b3 or b4;
}

bool GetSensorCorners(std::string ladder_name, std::string sensor_name,
			TVector3* u1, TVector3* u2, TVector3* u3, TVector3* u4,
			TVector3* v1, TVector3* v2, TVector3* v3, TVector3* v4)
{
	bool b1 = true;
	bool b2 = true;
	bool b3 = true;
	bool b4 = true;

	std::map<std::string, std::tuple<bool*, TVector3*, TVector3*>> m = 
	{
		{"crss_1", {&b1, u1, v1}},
		{"crss_2", {&b2, u2, v2}},
		{"crss_3", {&b3, u3, v3}},
		{"crss_4", {&b4, u4, v4}}
	};

	for(Long64_t n = 0; n < intt_geo_tree->GetEntriesFast(); ++n)
	{
		intt_geo_tree->GetEntry(n);
		if(name.find(ladder_name) == std::string::npos)continue;
		if(name.find(sensor_name) == std::string::npos)continue;

		for(auto itr = m.begin(); itr != m.end(); ++itr)
		{
			if(name.find(itr->first) != std::string::npos)
			{
				*std::get<0>(itr->second) = false;

				std::get<1>(itr->second)->SetX(mx);
				std::get<1>(itr->second)->SetY(my);
				std::get<1>(itr->second)->SetZ(mz);

				std::get<2>(itr->second)->SetX(nx);
				std::get<2>(itr->second)->SetY(ny);
				std::get<2>(itr->second)->SetZ(nz);
			}
		}
	}

	return b1 or b2 or b3 or b4;
}

TMatrix GetTransform(TVector3 c1, TVector3 c2, TVector3 c3, TVector3 c4)
{
	TVector3 o = (c1 + c2 + c3 + c4) * 0.25;

	TVector3 z = ((c3 - c1).Unit() - (c4 - c2).Unit()).Unit();
	TVector3 x = ((c3 - c1).Unit() + (c4 - c2).Unit()).Unit();
	TVector3 y = z.Cross(x).Unit();

	TMatrix T(4, 4);

	T[0][0] = x.X();	T[0][1] = y.X();	T[0][2] = z.X();	T[0][3] = o.X();
	T[1][0] = x.Y();	T[1][1] = y.Y();	T[1][2] = z.Y();	T[1][3] = o.Y();
	T[2][0] = x.Z();	T[2][1] = y.Z();	T[2][2] = z.Z();	T[2][3] = o.Z();
	T[3][0] = 0.0;		T[3][1] = 0.0;		T[3][2] = 0.0;		T[3][3] = 1.0;

	return T;
}

void SetTransformParams(TMatrix T, float* x, float* y, float* z, float* a, float* b, float* g)
{
	*x = T[0][3];
	*y = T[1][3];
	*z = T[2][3];
	*a = atan(-T[1][2] / T[2][2]);
	*b = atan(T[0][2] / sqrt(1 - T[0][2] * T[0][2]));
	*g = atan(-T[0][1] / T[0][0]);
	
}

void PrintTransform(TMatrix T, int t)
{
	for(int i = 0; i < t; ++i)std::cout << "\t";
	std::cout << T[0][0] << "\t" << T[0][1] << "\t" << T[0][2] << "\t" << T[0][3] << std::endl;
	for(int i = 0; i < t; ++i)std::cout << "\t";
	std::cout << T[1][0] << "\t" << T[1][1] << "\t" << T[1][2] << "\t" << T[1][3] << std::endl;
	for(int i = 0; i < t; ++i)std::cout << "\t";
	std::cout << T[2][0] << "\t" << T[2][1] << "\t" << T[2][2] << "\t" << T[2][3] << std::endl;
	for(int i = 0; i < t; ++i)std::cout << "\t";
	std::cout << T[3][0] << "\t" << T[3][1] << "\t" << T[3][2] << "\t" << T[3][3] << std::endl;
}
