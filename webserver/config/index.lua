AddHeader("Server", "TyRoXx WebServer 0.1")
AddHeader("Content-Type", "text/html")

Raw("<html><head><title>WebServer</title></head><body>")

for i = 0, 9 do
	Write(tostring(i))
	Raw("<br>")
end

Write("<p>escaped</p>")

Raw("</body></html>")
