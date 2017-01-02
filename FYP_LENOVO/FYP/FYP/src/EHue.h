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

	EHue_Grey,

	EHue_Count
};

Vec3b hueToVec3b(const EHue i_eHue)
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
	case EHue_Grey:
		return Vec3b(127, 127, 127);
	}

	return Vec3b(255, 255, 255);
}

Vec3b hueToVec3b(const int i_iHueEnumVal)
{
	return hueToVec3b((EHue)i_iHueEnumVal);
}

Vec3b colorFromLinear(const float i_fLinearHue)
{
	const float fIntensity = 1.f;

	float fBFactor = 0.f;
	float fGFactor = 0.f;
	float fRFactor = 0.f;

	float fStep = 1.f / EHue_Pink;

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

void normalise(Vec3b& io_vColor)
{
	const int iTotal = io_vColor[0] + io_vColor[1] + io_vColor[2];
	float fRatioB = (float)io_vColor[0] / (float)iTotal;
	float fRatioG = (float)io_vColor[1] / (float)iTotal;
	float fRatioR = (float)io_vColor[2] / (float)iTotal;

	io_vColor[0] = (int)(fRatioB * 255.f);
	io_vColor[1] = (int)(fRatioG * 255.f);
	io_vColor[2] = (int)(fRatioR * 255.f);
}

void normaliseHLS(Vec3b& io_vColor)
{
	io_vColor[1] = 127;
	io_vColor[2] = 255;
}

EHue vecToHue(const Vec3b& i_vbColor)
{
	// Convert input color to float for arithmetic operation
	const Vec3f i_vfColor = (Vec3f)i_vbColor; 

	// Track the hue data that is the closest match for the input
	EHue eClosest;
	double dClosest = (double)FLT_MAX;
	for(int iHue = EHue_Red; iHue <= EHue_Pink; ++iHue)
	{
		EHue eHue = (EHue)iHue;

		// Get the normalised color value for this hue
		Vec3b vHue = hueToVec3b(eHue);
		normalise(vHue);

		// Get the difference between this hue and the input color
		const Vec3f vDiff = (Vec3f)vHue - i_vfColor;
		double dDiff = norm(vDiff);
		if(dDiff < dClosest)
		{
			dClosest = dDiff;
			eClosest = eHue;
		}
	}

	if(dClosest < 75.f)
	{
		return eClosest;
	}
	else
	{
		return EHue_Grey;
	}
}

void snapToHue(Vec3b& io_vColor)
{
	io_vColor = hueToVec3b(vecToHue(io_vColor));
}

// i_iChannelDivisions: The number of fractions of 255 we will snap each channel to
void snapColors(Vec3b& io_vColor, const int i_iChannelDivisions)
{
	io_vColor[0] = (char)(io_vColor[0] / i_iChannelDivisions);
	io_vColor[0] = (char)(io_vColor[0] * i_iChannelDivisions);

	io_vColor[1] = (char)(io_vColor[1] / i_iChannelDivisions);
	io_vColor[1] = (char)(io_vColor[1] * i_iChannelDivisions);

	io_vColor[2] = (char)(io_vColor[2] / i_iChannelDivisions);
	io_vColor[2] = (char)(io_vColor[2] * i_iChannelDivisions);
}

float lerp(float a, float b, float mmin, float mmax, float m)
{
	// TODO
	float factor = (m - mmin) / (mmax - mmin);
	return a + (b - a) * factor;
}

// i_iChannelDivisions: The number of fractions of 255 we will snap each channel to
void snapColorsHLS(Vec3b& io_vColor, const int i_iChannelDivisions)
{
	float H = io_vColor[0];
	float L = io_vColor[1];
	float S = io_vColor[2];

	// Hue
	io_vColor[0] = (char)(io_vColor[0] / i_iChannelDivisions);
	io_vColor[0] = (char)(io_vColor[0] * i_iChannelDivisions);

	// Determine if we're black/grey/white
	if(S < 25)
	{
		// Definitely desaturated
		if(L < 50)
		{
			// Set to black
			io_vColor[1] = 0;
			io_vColor[2] = 0;
		}
		else if (L > 200)
		{
			// Set to white
			io_vColor[1] = 255;
			io_vColor[2] = 0;
		}
		else
		{
			// Set to grey
			io_vColor[1] = 127;
			io_vColor[2] = 0;
		}
	}
	else
	{
		// Saturated, but may still be black/white 45S 15L
		if(L < 50)
		{
			// Is black if below S threshold
			bool b = false;
			if(fabsf(L - 38.5f) < 1.f)
			{
				if(fabsf(S - 122.7f) < 1.f)
				{
					b = true;
				}
			}
			float fSThreshold = lerp(25, 255, 50, 18, L);
			if(S < fSThreshold)
			{
				// Set to black
				io_vColor[1] = 0;
				io_vColor[2] = 0;
			}
		}
		else if(L > 205)
		{
			// Is white if below S threshold
			float fSThreshold = lerp(25, 255, 205, 237, L);
			if(S < fSThreshold)
			{
				// Set to white
				io_vColor[1] = 255;
				io_vColor[2] = 0;
			}
		}
		else
		{
			// Normal color; snap normally
			// Lightness
			io_vColor[1] = 127;

			// Saturation
			io_vColor[2] = 255;
		}
	}
}