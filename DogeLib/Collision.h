#pragma once

bool colPolyBb(Poly *const p1, Poly *const p2)
{
	const CoordPair mm1 = polyBbMinMax(p1);
	const CoordPair mm2 = polyBbMinMax(p2);
	return (
		mm1.max.x >= mm2.min.x && mm1.min.x <= mm2.max.x &&
		mm1.max.y >= mm2.min.y && mm1.min.y <= mm2.max.y
	);
}
