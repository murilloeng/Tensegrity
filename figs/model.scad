//data
nc = 5;
t = 2.00e-02;
r = 5.00e-01;
e = 2.50e-01;
rc = 5.00e-03;
ht = 1.00e+00;
hc = 5.00e-01;
hr = (ht + hc) / 2;
pi = 3.14159265359;

//base
module base()
{
	color([0, 0, 1])
	{
		translate([0, 0, -t / 2])
		{
			cylinder(h = t, r = r, $fn = 40);
		}
		translate([e - t / 2, -t / 2, 0])
		{
			cube([t, t, hr + t / 2]);
		}
		translate([0, -t / 2, hr - t / 2])
		{
			cube([e, t, t]);
		}
	}
}

base();
translate([0, 0, ht])
{
	rotate([0, 0, 180])
	{
		rotate([180, 0, 0])
		{
			base();
		}
	}
}
color([1, 0, 0])
{
	translate([0, 0, hr - hc])
	{
		cylinder(h = hc, r = rc, $fn = 20);
	}
	for(i = [0 : nc])
	{
		rotate([0, 0, 360 * i / nc])
		{
			translate([r, 0, 0])
			{
				cylinder(h = ht, r = rc, $fn = 20);
			}
		}
	}
}
translate([rc / 2, 0, hr - hc / 2])
{
	rotate([90, 0, 0])
	{
		scale([1, 1, 0.01])
		{
			color([0, 0, 0])
			{
				text("hc", size = 0.1, valign = "center");
			}
		}
	}
}
translate([e + t / 2, 0, hr / 2])
{
	rotate([90, 0, 0])
	{
		scale([1, 1, 0.01])
		{
			color([0, 0, 0])
			{
				text("hr", size = 0.1, valign = "center");
			}
		}
	}
}
translate([r + t / 2, 0, ht / 2])
{
	rotate([90, 0, 0])
	{
		scale([1, 1, 0.01])
		{
			color([0, 0, 0])
			{
				text("ht", size = 0.1, valign = "center");
			}
		}
	}
}