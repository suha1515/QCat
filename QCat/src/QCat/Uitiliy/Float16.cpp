#include "qcpch.h"
#include "Float16.h"

namespace QCat
{
    void Float16::setFloat(float input) {
        union
        {
            float f;
            unsigned int i;
        } value = { input };
        unsigned int i = value.i;
        int s = (i >> 16) & 0x00008000;                    // sign
        int e = ((i >> 23) & 0x000000ff) - (127 - 15);      // exponent
        int f = i & 0x007fffff;                            // fraction
                                                                     // need to handle NaNs and Inf?
        if (e <= 0)
        {
            if (e < -10)
            {
                if (s)                                               // handle -0.0
                    h = 0x8000;
                else
                    h = 0;
            }

            f = (f | 0x00800000) >> (1 - e);
            h = s | (f >> 13);
        }
        else if (e == 0xff - (127 - 15))
        {
            if (f == 0)                                             // Inf
                h = s | 0x7c00;
            else
            {                                                       // NAN
                f >>= 13;
                h = s | 0x7c00 | f | (f == 0);
            }
        }
        else
        {
            if (e > 30)                                             // Overflow
                h = s | 0x7c00;

            h = s | (e << 10) | (f >> 13);
        }
    }

    float Float16::getFloat() const
    {
        union
        {
            float f;
            unsigned int i;
        } value;

        int s = (h >> 15) & 0x00000001;                            // sign
        int e = (h >> 10) & 0x0000001f;                            // exponent
        int f = h & 0x000003ff;                                       // fraction

                                                                   // need to handle 7c00 INF and fc00 -INF?
        if (e == 0)
        {
            // need to handle +-0 case f==0 or f=0x8000?
            if (f == 0)                                            // Plus or minus zero
                value.i = s << 31;
            else
            {                                                      // Denormalized number -- renormalize it
                while (!(f & 0x00000400))
                {
                    f <<= 1;
                    e -= 1;
                }
                e += 1;
                f &= ~0x00000400;
            }
        }
        else if (e == 31)
        {
            if (f == 0)                                             // Inf
                value.i = (s << 31) | 0x7f800000;
            else                                                    // NaN
                value.i = (s << 31) | 0x7f800000 | (f << 13);
        }

        e = e + (127 - 15);
        f = f << 13;

        value.i = ((s << 31) | (e << 23) | f);

        return value.f;
    }
}