#include <math.h>

void calcSlope(int x1, int y1, int x2, int y2, float *dx, float *dy)
{
	int steps = std::max(std::abs(x1 - x2), std::abs(y1 - y2));

	if (steps == 0)
	{
		*dx = *dy = 0;
		return;
	}

	*dx = (x1 - x2);
	*dx /= steps;

	*dy = (y1 - y2);
	*dy /= steps;
}