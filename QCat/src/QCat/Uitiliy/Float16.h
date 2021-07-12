#pragma once
namespace QCat
{
    class Float16
    {
    public:
        Float16() { }
        Float16(const Float16& h) : h(h.h) { }
        Float16(int i) : h((unsigned short)i) { }
        Float16(float f) { setFloat(f); }

        Float16& operator=(Float16 v) { h = v.h; return *this; }
        Float16& operator=(float f) { setFloat(f); return *this; }
        Float16& operator=(unsigned short s) { h = s; return *this; }

        operator float() const { return getFloat(); }
        operator unsigned short() const{ return h; }

        void setFloat(float input);
        float getFloat() const;

    private:
        unsigned short h = 0;
    };

}
