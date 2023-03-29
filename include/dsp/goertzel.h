#ifndef GOERTZEL_H
#define GOERTZEL_H


#include <cstdint>
//#include "gcem.hpp"
#include <cmath>
#include <cmath>



template <uint32_t N,
          uint32_t fSample,
          uint32_t fTone>
struct GoertzelT {

    constexpr static float cPi = 3.14159265f;
    constexpr static float tone = static_cast<float>(fTone);
    constexpr static float sample = static_cast<float>(fSample);

    float adder;

	constexpr static float k = tone * (N) / sample;

        //    float k1 = k;

    float z1 { 0.0f };
    float z2 { 0.0f };
    uint32_t counter{ N - 1 };

    constexpr static float omega = 2.0 * cPi * k / N;
    constexpr static float ci = sinf(omega);
    constexpr static float cr = cosf(omega);

    constexpr static float alpha = 2.0 * cr;
    constexpr static float scale = N / 2.0;

    float rr {0.0f};
    float ri {0.0f};


    //cplx out;

    ///
    /// \brief Add next sample to algorithm
    /// \param sample
    /// \return true result is ready false not yet
    ///
    bool addSample(float sample)
    {

        adder = sample + z1 * alpha - z2;

        if (!counter ) {
            rr = (adder * cr - z1) / scale;
            ri = (adder * ci) / scale;
            z2 = z1 = 0;
            counter = N - 1;
            return true;
        }

        z2 = z1;
        z1 = adder;
        --counter;
        return false;
    }

    ///
    /// \brief Получение значения бина
    /// \return комплексное значение бина
    ///
    float getBinAmpl()
    {
        return sqrt(rr * rr + ri * ri);

    }

	double getBinPhase()
    {
        return atan2(ri,  rr);
    }

    ///
    /// \brief Инициализация алгоритма
    ///
    void init()
    {
        z1 = z2 = 0;
        counter = N - 1;
    }
};

template<typename T=float>
class GoertzelD {

	constexpr static T cPi = 3.14159265f;
  T tone;
  T sample;
	uint32_t N;

  T z1 { 0.0f };
  T z2 { 0.0f };
	uint32_t counter;

  T omega;
  T alpha;
  T scale;
  T k;

  T cr;
  T ci;

	//cplx out;

  void recalc()
  {
    counter = N -1;
//    counter = N;
    z1 = 0.0F;
    z2 = 0.0F;
    k = tone * (N) / sample;
    omega = 2.0F * cPi * k / N;
    alpha = 2.0F * cosf(omega);
    scale = N / 2.0F;
    cr = cosf(omega);
    ci =  sinf(omega);
    ri = 0.0F;
    rr = 0.0F;
  }


public:

  T rr {0.0f};
  T ri {0.0f};

	GoertzelD():
    tone(0),
    sample(0),
    N(0)
  {
    recalc();
  }

	GoertzelD(const uint32_t num,
	          const uint32_t fSample,
	          const uint32_t fTone):
    tone(fTone),
    sample(fSample),
    N(num)
  {
    recalc();
  }

	void config(const uint32_t num,
	            const uint32_t fSample,
	            const uint32_t fTone)
  {
    tone = fTone;
    N = num;
    sample = fSample;
    recalc();
  }


  void setBin(const uint32_t fTone)
  {
    tone = fTone;
    recalc();
  }

	void setNum(const uint32_t num)
  {
    N = num;
    recalc();
  }

	void setSampleRate(const uint32_t fSample)
  {
    sample = fSample;
    recalc();
  }

	///
	/// \brief Add next sample to algorithm
	/// \param sample
	/// \return true result is ready false not yet
	///
	bool addSample(float sample)
  {
//    Gpio<'F'>::setPin(0);
    float adder = sample + z1 * alpha - z2;

    if (!counter ) {
      rr = (adder * cr - z1) / scale;
      ri = (adder * ci) / scale;
      z2 = z1 = 0;
      counter = N - 1;
//        counter = N;

//        Gpio<'F'>::resetPin(0);


//      rr = 4; //QWERTY
//      ri = 5;

      return true;
    }

    z2 = z1;
    z1 = adder;
    --counter;

//    Gpio<'F'>::resetPin(0);
    return false;
  }


	///
	/// \brief Получение значения бина
	/// \return комплексное значение бина
	///
	float getBinAmpl()  const
  {
    return sqrtf(rr * rr + ri * ri);

  }

  float getBinRe()  const
  {
    return rr;

  }

	double getBinPhase() const
  {
    return atan2f(ri,  rr);

  }

	void init()
  {
    z1 = z2 = 0.0F;
    counter = N - 1;
//    counter = N;
  }
};


#endif // GOERTZEL_H
