def add(x: integer, y: integer): integer
	x + y
end

def main(): integer
	var x: integer = 0;
	var y, z: float;
	var str: string;
	input x, y, z, str;
	while (x < y and y <= 5.0) || y == z do
		if x >= 2 && (y < z or x <> z) then
			z = z - 1;
		else
			y = x;
		end
	end
	output x, y, z, str;
	output add(x + y) * z;
end