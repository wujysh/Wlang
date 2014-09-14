def test(a: integer, b: float, c: string): float
	var d: integer;
	var e: string;
	d = a;
	e = c;
	return b;
end

def test_without_string(a: integer, b: float): float
	var d: integer;
	d = a;
	return b;
end

def main(): integer
	test_without_string(1, 1.0);
	var t: string;
	t = "hello";
	test(1, 1.0, t);
	return 0;
end