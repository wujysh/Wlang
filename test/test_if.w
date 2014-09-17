def main(): integer
	var a: integer;
	if 1 < 2 && (3 > 2 || (4<2 && 5>3)) then
		a = 1;
	end
	if 1 > 2 then
		a = -1;
	end
	if 1 < 5 && 2 > 1 then
		a = 1;
	end
	if 1 < 5 && (2 > 1 && 4 < 6) then
		a = 1;
	end
	return a;
end


