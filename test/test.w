function add()
begin
	def x, y, z as integer;
	z = x + y;
end
end function

function main()
begin
	def x as integer;
	def y,z as float;
	def str as string;
	input x, y, z, str;
	while (x < y and y <= 5.0) or y == z do
	begin
		if x >= 2 and (y < z or x <> z)
		begin
			z = z - 1;
		end
		else
		begin
			y = x;
		end
	end
	output x, y, z, str;
	output (x+y)*z;
end
end function
