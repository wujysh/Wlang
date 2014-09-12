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
	z = 6;
	x = z + 1;
	y = z - 1;
	z = 1 + 1;
	
	add(1, 3), add(x, y);
	z = add(x, y);
	return z;
end


