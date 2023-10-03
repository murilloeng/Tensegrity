//std
#include <cmath>
#include <cstdlib>

class Tensegrity
{
private:
	/* data */
public:
	Tensegrity(/* args */);
	~Tensegrity();
};

Tensegrity::Tensegrity(/* args */)
{
}

Tensegrity::~Tensegrity()
{
}


//data
static unsigned nc = 3;
static const double e = 5.50e-02;
static const double Hc = 4.00e-02;
static const double Hr = 8.50e-02;
static const double Rr = 1.00e-01;
static const double dc = 3.70e-04;
static const double Ec = 8.00e+10;

static const double Ht = 2 * Hr - Hc;
static const double Ac = M_PI * dc * dc / 4;

int main(void)
{
	//return
	return EXIT_SUCCESS;
}