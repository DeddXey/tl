#include "dsp/goertzel.h"
#include "gpio.h"
//------------------------------------------------------------------------
GoertzelD::GoertzelD():
    tone(0),
    sample(0),
    N(0)
{
    recalc();
}

//------------------------------------------------------------------------
GoertzelD::GoertzelD(const uint32_t num,
                     const uint32_t fSample,
                     const uint32_t fTone):
    tone(fTone),
    sample(fSample),
    N(num)
{
    recalc();
}

//------------------------------------------------------------------------
void GoertzelD::config(const uint32_t num,
                       const uint32_t fSample,
                       const uint32_t fTone)
{
    tone = fTone;
    N = num;
    sample = fSample;
    recalc();
}

//------------------------------------------------------------------------
void GoertzelD::setBin(const uint32_t fTone)
{
    tone = fTone;
    recalc();
}

//------------------------------------------------------------------------
void GoertzelD::setNum(const uint32_t num)
{
    N = num;
    recalc();
}

//------------------------------------------------------------------------
void GoertzelD::setSampleRate(const uint32_t fSample)
{
    sample = fSample;
    recalc();
}

//------------------------------------------------------------------------
bool GoertzelD::addSample(float sample)
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
        return true;
    }

    z2 = z1;
    z1 = adder;
    --counter;

//    Gpio<'F'>::resetPin(0);
    return false;
}

//------------------------------------------------------------------------
void GoertzelD::recalc()
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

//------------------------------------------------------------------------
float GoertzelD::getBinAmpl()  const
{
    return sqrtf(rr * rr + ri * ri);

}

//------------------------------------------------------------------------
double GoertzelD::getBinPhase() const
{
    return atan2f(ri,  rr);
}

//------------------------------------------------------------------------
void GoertzelD::init()
{
    z1 = z2 = 0.0F;
    counter = N - 1;
//    counter = N;
}

