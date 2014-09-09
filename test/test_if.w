def main(): integer
	if 1 < 2 then
		return 1;
	end
	if 1 > 2 then
		return -1;
	end
	if 1 < 5 && 2 > 1 then
		return 1;
	end
	if 1 < 5 && (2 > 1 && 4 < 6) then
		return 1;
	end
	return 0;
end


