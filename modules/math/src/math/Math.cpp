//---------------------------------------------------------------------------

#include <math\Matrix.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	template struct MathConstants<float>;
	template struct MathConstants<double>;
	template struct MathConstants<Vector<float>>;
	template struct MathConstants<Vector<double>>;

	const float  MathConstants<float> ::one			= 1.0f;
	const double MathConstants<double>::one			= 1.0;
	const float  MathConstants<float> ::pi			= 3.141592653f;
	const double MathConstants<double>::pi			= 3.14159265358979323846;
	const float  MathConstants<float> ::pi2			= 6.283185307f;
	const double MathConstants<double>::pi2			= 6.28318530717958647692;
	const float  MathConstants<float> ::half_pi		= 1.570796326f;
	const double MathConstants<double>::half_pi		= 1.57079632679489661923;
	const float  MathConstants<float> ::half_pi3	= 4.712388980f;
	const double MathConstants<double>::half_pi3	= 4.71238898038468985769;
	const float  MathConstants<float> ::inv_pi		= 0.318309886f;
	const double MathConstants<double>::inv_pi		= 0.318309886183790671538;
	const float  MathConstants<float> ::inv_pi2		= 0.159154943f;
	const double MathConstants<double>::inv_pi2		= 0.159154943091895335769;
	const float  MathConstants<float> ::degree180	= 180.0f;
	const double MathConstants<double>::degree180	= 180.0;
	const float  MathConstants<float> ::degree360	= 360.0f;
	const double MathConstants<double>::degree360	= 360.0;
	const float  MathConstants<float> ::eps			= 1.192092896e-07f;
	const double MathConstants<double>::eps			= 2.2204460492503131e-016;
	const float  MathConstants<float> ::eps2		= Math<float> ::eps * Math<float> ::eps;
	const double MathConstants<double>::eps2		= Math<double>::eps * Math<double>::eps;
	const float  MathConstants<float> ::eps3		= Math<float> ::eps * Math<float> ::eps * Math<float> ::eps;
	const double MathConstants<double>::eps3		= Math<double>::eps * Math<double>::eps * Math<double>::eps;
	const float  MathConstants<float> ::infinity	= std::numeric_limits<float>::infinity();
	const double MathConstants<double>::infinity	= std::numeric_limits<double>::infinity();

	const float  MathConstants<float> ::degreetorad = Math<float> ::pi / Math<float> ::degree180;
	const double MathConstants<double>::degreetorad = Math<double>::pi / Math<double>::degree180;
	const float  MathConstants<float> ::radtodegree = Math<float> ::degree180 / Math<float> ::pi;
	const double MathConstants<double>::radtodegree = Math<double>::degree180 / Math<double>::pi;

	const float MathCoefficients<float>::sin[] = {
		-0.16666666666666666666666666666667f,
		+0.00833333333333333333333333333333f,
		-1.984126984126984126984126984127e-4f,
		+2.7557319223985890652557319223986e-6f,
		-2.5052108385441718775052108385442e-8f
	};

	const double MathCoefficients<double>::sin[] = {
		-0.16666666666666666666666666666667,
		+0.00833333333333333333333333333333,
		-1.984126984126984126984126984127e-4,
		+2.7557319223985890652557319223986e-6,
		-2.5052108385441718775052108385442e-8
	};

	const float MathCoefficients<float>::cos[] = {
		-0.5f,
		+0.04166666666666666666666666666667f,
		-0.00138888888888888888888888888889f,
		+2.4801587301587301587301587301587e-5f,
		-2.7557319223985890652557319223986e-7f
	};

	const double MathCoefficients<double>::cos[] = {
		-0.5,
		+0.04166666666666666666666666666667,
		-0.00138888888888888888888888888889,
		+2.4801587301587301587301587301587e-5,
		-2.7557319223985890652557319223986e-7
	};

	template struct Vector<float>;
	template struct Vector<double>;
	template struct Matrix<float>;
	template struct Matrix<double>;
	template struct Matrix2x2<float>;
	template struct Matrix2x2<double>;

	const FloatVector  FloatVector ::positiveX  { 1,  0,  0,  0 };
	const DoubleVector DoubleVector::positiveX  { 1,  0,  0,  0 };
	const FloatVector  FloatVector ::positiveY  { 0,  1,  0,  0 };
	const DoubleVector DoubleVector::positiveY  { 0,  1,  0,  0 };
	const FloatVector  FloatVector ::positiveZ  { 0,  0,  1,  0 };
	const DoubleVector DoubleVector::positiveZ  { 0,  0,  1,  0 };
	const FloatVector  FloatVector ::positiveW  { 0,  0,  0,  1 };
	const DoubleVector DoubleVector::positiveW  { 0,  0,  0,  1 };
	const FloatVector  FloatVector ::negativeX  {-1,  0,  0,  0 };
	const DoubleVector DoubleVector::negativeX  {-1,  0,  0,  0 };
	const FloatVector  FloatVector ::negativeY  { 0, -1,  0,  0 };
	const DoubleVector DoubleVector::negativeY  { 0, -1,  0,  0 };
	const FloatVector  FloatVector ::negativeZ  { 0,  0, -1,  0 };
	const DoubleVector DoubleVector::negativeZ  { 0,  0, -1,  0 };
	const FloatVector  FloatVector ::negativeW  { 0,  0,  0, -1 };
	const DoubleVector DoubleVector::negativeW  { 0,  0,  0, -1 };

	const FloatVector  FloatVector ::zero		{ 0,  0,  0,  0 };
	const DoubleVector DoubleVector::zero		{ 0,  0,  0,  0 };
	const FloatVector  FloatVector ::one		{ 1,  1,  1,  1 };
	const DoubleVector DoubleVector::one		{ 1,  1,  1,  1 };
	const FloatVector  FloatVector ::two		{ 2,  2,  2,  2 };
	const DoubleVector DoubleVector::two		{ 2,  2,  2,  2 };
	const FloatVector  FloatVector ::oneXYZ		{ 1,  1,  1,  0 };
	const DoubleVector DoubleVector::oneXYZ		{ 1,  1,  1,  0 };
	const FloatVector  FloatVector ::twoXYZ		{ 2,  2,  2,  0 };
	const DoubleVector DoubleVector::twoXYZ		{ 2,  2,  2,  0 };
	const FloatVector  FloatVector ::minusOne	{-1, -1, -1, -1 };
	const DoubleVector DoubleVector::minusOne	{-1, -1, -1, -1 };

	const FloatVector  FloatVector ::half		{.5f,.5f,.5f,.5f};
	const DoubleVector DoubleVector::half		{.5, .5, .5, .5 };

#define implement_vector_constants(constant)																									\
	const FloatVector  FloatVectorMath ::constant { FloatMath::constant,  FloatMath::constant,  FloatMath::constant,  FloatMath::constant  };	\
	const DoubleVector DoubleVectorMath::constant { DoubleMath::constant, DoubleMath::constant, DoubleMath::constant, DoubleMath::constant };	\

	implement_vector_constants(one);
	implement_vector_constants(pi);
	implement_vector_constants(pi2);
	implement_vector_constants(half_pi);
	implement_vector_constants(half_pi3);
	implement_vector_constants(inv_pi);
	implement_vector_constants(inv_pi2);
	implement_vector_constants(degree180);
	implement_vector_constants(degree360);
	implement_vector_constants(eps);
	implement_vector_constants(eps2);
	implement_vector_constants(eps3);
	implement_vector_constants(infinity);
	implement_vector_constants(degreetorad);
	implement_vector_constants(radtodegree);

#undef implement_vector_constants

#define implement_vector_coefs(coefs)															\
	const FloatVector FloatVectorCfs::coefs[] {													\
		{FloatCfs::coefs[0], FloatCfs::coefs[0], FloatCfs::coefs[0], FloatCfs::coefs[0]},		\
		{FloatCfs::coefs[1], FloatCfs::coefs[1], FloatCfs::coefs[1], FloatCfs::coefs[1]},		\
		{FloatCfs::coefs[2], FloatCfs::coefs[2], FloatCfs::coefs[2], FloatCfs::coefs[2]},		\
		{FloatCfs::coefs[3], FloatCfs::coefs[3], FloatCfs::coefs[3], FloatCfs::coefs[3]},		\
		{FloatCfs::coefs[4], FloatCfs::coefs[4], FloatCfs::coefs[4], FloatCfs::coefs[4]},		\
	};																							\
																								\
	const DoubleVector DoubleVectorCfs::coefs[] {												\
		{DoubleCfs::coefs[0], DoubleCfs::coefs[0], DoubleCfs::coefs[0], DoubleCfs::coefs[0]},	\
		{DoubleCfs::coefs[1], DoubleCfs::coefs[1], DoubleCfs::coefs[1], DoubleCfs::coefs[1]},	\
		{DoubleCfs::coefs[2], DoubleCfs::coefs[2], DoubleCfs::coefs[2], DoubleCfs::coefs[2]},	\
		{DoubleCfs::coefs[3], DoubleCfs::coefs[3], DoubleCfs::coefs[3], DoubleCfs::coefs[3]},	\
		{DoubleCfs::coefs[4], DoubleCfs::coefs[4], DoubleCfs::coefs[4], DoubleCfs::coefs[4]},	\
	};		
	
	implement_vector_coefs(sin);
	implement_vector_coefs(cos);

#undef implement_vector_coefs

	const Matrix2x2<float> Matrix2x2<float>::initial {
		 1,  0,
		 0,  1
	};

	const Matrix2x2<double> Matrix2x2<double>::initial {
		 1,  0,
		 0,  1
	};

	const Matrix2x2<float> Matrix2x2<float>::negative {
		-1,  0,
		 0, -1
	};

	const Matrix2x2<double> Matrix2x2<double>::negative {
		-1,  0,
		 0, -1
	};

	const Matrix2x2<float> Matrix2x2<float>::inversemask {
		 1, -1,
		-1,  1
	};

	const Matrix2x2<double> Matrix2x2<double>::inversemask {
		 1, -1,
		-1,  1
	};
}