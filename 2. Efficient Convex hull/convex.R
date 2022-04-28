#! /usr/bin/env Rscript
png("convex.png", width=700, height=700)
plot(1:10000, 1:10000, type="n")

#points
points(1059,1409)
points(1059,9315)
points(3950,6954)
points(4168,8515)
points(6868,2338)
points(6991,3456)
points(7396,9661)
points(7767,3235)
points(9476,8024)
points(9629,1425)

#line segments
segments(1059,1409,1059,9315)
segments(1059,9315,7396,9661)
segments(7396,9661,9476,8024)
segments(9476,8024,9629,1425)
segments(9629,1425,1059,1409)
dev.off()
