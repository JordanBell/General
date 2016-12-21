using namespace std;
using namespace cv;

enum EHue
{
	EHue_Red = 1,
	EHue_Orange,
	EHue_Yellow,
	EHue_YellowGreen,
	EHue_Green,
	EHue_Turquoise,
	EHue_Cyan,
	EHue_Indigo,
	EHue_Blue,
	EHue_Purple,
	EHue_Violet,
	EHue_Pink,

	EHue_Count
};

Vec3b getEHueToVec3b(const EHue i_eHue)
{
	Vec3b r_vColor;
	switch(i_eHue)
	{
	case EHue_Red:
		return Vec3b(0, 0, 255);
	case EHue_Orange:
		return Vec3b(0, 127, 255);
	case EHue_Yellow:
		return Vec3b(0, 255, 255);
	case EHue_YellowGreen:
		return Vec3b(0, 255, 127);
	case EHue_Green:
		return Vec3b(0, 255, 0);
	case EHue_Turquoise:
		return Vec3b(127, 255, 0);
	case EHue_Cyan:
		return Vec3b(255, 255, 0);
	case EHue_Indigo:
		return Vec3b(255, 127, 0);
	case EHue_Blue:
		return Vec3b(255, 0, 0);
	case EHue_Purple:
		return Vec3b(255, 0, 127);
	case EHue_Violet:
		return Vec3b(255, 0, 255);
	case EHue_Pink:
		return Vec3b(127, 0, 255);
	}

	return Vec3b(255, 255, 255);
}

Vec3b colorFromLinear(const float i_fLinearHue)
{
	const float fIntensity = 1.f;

	float fBFactor = 0.f;
	float fGFactor = 0.f;
	float fRFactor = 0.f;

	float fStep = 1.f / 12.f;

	// Determine blue value
	{
		if(i_fLinearHue >= fStep * EHue_Cyan && i_fLinearHue <= fStep * EHue_Violet)
		{
			fBFactor = 1.f;
		}
		else if (i_fLinearHue > fStep * EHue_Violet)
		{
			fBFactor = 1.f - (i_fLinearHue - fStep * EHue_Violet) / (2.f * fStep);
		}
		else if (i_fLinearHue <= fStep * EHue_Red)
		{
			fBFactor = 0.5f - 0.5f * (i_fLinearHue / fStep);
		}
		else if (i_fLinearHue >= fStep * EHue_Green)
		{
			fBFactor = (i_fLinearHue - fStep * EHue_Green) / (2.f * fStep);
		}
	}

	// Determine green value
	{
		if (i_fLinearHue > fStep * EHue_Red && i_fLinearHue < fStep * EHue_Yellow)
		{
			fGFactor = (i_fLinearHue - fStep * EHue_Red) / (2.f * fStep);
		}
		else
		{
			if(i_fLinearHue >= fStep * EHue_Yellow)
			{
				if(i_fLinearHue < fStep * EHue_Cyan)
				{
					fGFactor = 1.f;
				}
				else if (i_fLinearHue < fStep * EHue_Blue)
				{
					fGFactor = 1.f - ((i_fLinearHue - fStep * EHue_Cyan) / (2.f * fStep));
				}
			}
		}
	}

	// Determine red value
	{
		if(i_fLinearHue <= fStep * EHue_Yellow || i_fLinearHue >= fStep * EHue_Violet)
		{
			fRFactor = 1.f;
		}
		else if (i_fLinearHue <= fStep * EHue_Green)
		{
			fRFactor = 1.f - ((i_fLinearHue - fStep * EHue_Yellow) / (2.f * fStep));
		}
		else if (i_fLinearHue > fStep * EHue_Blue)
		{
			fRFactor = (i_fLinearHue - fStep * EHue_Blue) / (2.f * fStep);
		}
	}

	fBFactor = std::max(fBFactor, 0.f);
	fGFactor = std::max(fGFactor, 0.f);
	fRFactor = std::max(fRFactor, 0.f);

	return Vec3b(int(fBFactor * fIntensity * 255.f), int(fGFactor * fIntensity * 255.f), int(fRFactor * fIntensity * 255.f));
}