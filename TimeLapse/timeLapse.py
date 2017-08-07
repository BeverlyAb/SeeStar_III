#Some commands to combine the images and create a timelapse video

#sudo apt-get install ffmpeg
#https://ubuntuforums.org/showthread.php?t=2022316

# Choose one of the three and type into command line within the Timelapse dir
ffmpeg -r 25 -pattern_type glob -i '*.JPG' -c:v copy output.avi # Change -r 25 to define the frame rate. 25 here means 25 fps.
ffmpeg -r 25 -pattern_type glob -i '*.JPG' -c:v mjpeg -q:v 2 output.avi # -q:v can get a value between 2-31. 2 is best quality and bigger size, 31 is worst quality and least size)
ffmpeg -r 25 -pattern_type glob -i '*.JPG' -c:v ljpeg output.avi # Lossless jpeg resulting in a huuuuuuuge file. Even larger in size than the sum of all of the input pictures together in the case of jpg images.

#Optional alternative. Note: this step takes awhile
ffmpeg -i output.avi -c:v libx264 -preset slow -crf 15 output-final.mkv
