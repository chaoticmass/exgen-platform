
c = container:new();

c.top = 0;
c.left = 0;
c.width = screen_width();
c.height = screen_height();

c2 = container:new();
c2.parent = c;

echo(c2.parent.children);
echo("\n");