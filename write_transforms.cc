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

	FILE* file;
	file = fopen("intt_transforms.txt", "w");

	fprintf(file, "%s\t\t%s\t\t%s\t\t%s\n", "Name", "dx (mm)", "dy (mm)", "dz (mm)");
	for(Long64_t n = 0; n < transforms_tree->GetEntriesFast(); ++n)
	{
		transforms_tree->GetEntry(n);

		if(!(n % 4))fprintf(file, "\n%s\n", name.substr(0, 6).c_str());
		fprintf(file, "\t%s\t%+11.8f\t%+11.8f\t%+13.8f\n", name.substr(7).c_str(), dx_m, dy_m, dz_m);
	}

	fclose(file);
}
