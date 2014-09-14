def test(a: integer, b: float, c: string): float
	var d: integer;
	var e: string;
	d = a;
	e = c;
	return b;
end

def main(): integer
	test(1, 1.0, "hello");
	return 0;
end