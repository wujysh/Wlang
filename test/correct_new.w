/*  
 *	Wlang test file
 */
def add(x: integer, y: integer): integer
	var z: integer;
	z = 1 + 1;  // z = 2;
	return z;
end

def empty(): void
	return;
end

def main(): integer
	var x: integer;
	var y, z: integer;
	var str: float;
	z = 5;
	input x, y, z, str;
	add(1, 3), add(x, y);
	output add(1, 2);
	if x >= 0 && add(x, y) <= 100 || x <> y then
		empty();
		if z == z then
			y = y + z;
		else
			z = add(x, y);
		end
	else
		x > 0;
	end
	while x <= y do
		x = x + 1;
	end
	x + 5, 5, x * y;
	return x;
end


