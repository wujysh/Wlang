def add(x: integer, y: integer): integer
	z = x + y;
end

def empty(): float
	
end

def main(): integer
	var x: integer = 0;
	var y, z: float;
	var str: string;
	input x, y, z, str;
	if (x < 2) then
		x = x * 2;
	end
	if (x < 2) then
		x = x * 2;
	else
	    y = -1;
	    z = .1;
	    z = 1.1E+10;
	end
	while (x < y and y <= 5.0) || y == z do
		if x >= 2 && (y < z or x <> z) then
			z = z - 1;
		else
			y = x;
		end
	end
	output x, y, z, str;
	output (x+y), z;
end


