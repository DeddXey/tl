#ifndef GOERTZEL_H
#define GOERTZEL_H


#include <cstdint>
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
    uint32_t counter { N - 1 };

    constexpr static float omega = 2 * cPi * k / N;
    constexpr static float alpha = 2 * cosf(omega);
    constexpr static float scale = N / 2;

    float rr {0.0f};
    float ri {0.0f};

    constexpr static float cr = cosf(omega);
    constexpr static float ci =  sinf(omega);

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
        return sqrtf(rr * rr + ri * ri);

    }

	double getBinPhase()
    {
        return atan2f(ri,  rr);
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

class GoertzelD {

	constexpr static float cPi = 3.14159265f;
	float tone;
	float sample;
	uint32_t N;

	float z1 { 0.0f };
	float z2 { 0.0f };
	uint32_t counter;

	float omega;
	float alpha;
	float scale;
	float k;

	float cr;
	float ci;

	//cplx out;

	void recalc();


public:

	float rr {0.0f};
	float ri {0.0f};

	GoertzelD();

	GoertzelD(const uint32_t num,
	          const uint32_t fSample,
	          const uint32_t fTone);

	void config(const uint32_t num,
	            const uint32_t fSample,
	            const uint32_t fTone);

	void setBin(const uint32_t fTone);
	void setNum(const uint32_t num);
	void setSampleRate(const uint32_t fSample);

	///
	/// \brief Add next sample to algorithm
	/// \param sample
	/// \return true result is ready false not yet
	///
	bool addSample(float sample);

	///
	/// \brief Получение значения бина
	/// \return комплексное значение бина
	///
	float getBinAmpl()  const;

	double getBinPhase() const;

	void init();
};


#endif // GOERTZEL_H
