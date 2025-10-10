I = imread("engine.png");
if size(I,3) == 1
    I = repmat(I,[1,1,3]);
end
I = imresize(I, [512,512]);
imwrite(I, "512.ppm");
