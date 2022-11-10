Raw intt survey data is stored in the
	./intt_survey_data
directory.


Do
	root intt_survey_data.cc
to produce
	intt_survey_data.root
which is the raw survey data, reproduced in a .root file


Do
	root intt_geo.cc
to produce
	intt_geo.root
This contains the measured positions from the survey data,
but now the nominal positions are the exact ones from the
engineering schematics (the survey "nominals" are reference points
for the OGP machine")


Do
	root intt_transforms.cc
to produce
	intt_transforms.root
which contains the parameters for the affine transforms from
each sensor's local coordinate system to its parent ladder's
coordinate system. Does this for both the measured positions
and nominal positions in the intt_geo.root file, and computes
an additional transform which moves from the measured sensor
frame to the nominal sensor frame.
