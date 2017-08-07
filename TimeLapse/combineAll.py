import os
## This script is to fix the GoPro bug, where the GoPro creates multiple folders (100 - 999)
## and only stores a single image in each folder. Note that for the Open House example,
## the GoPro initially saved in the folder, 999GOPRO, which filled up after 1000 images, and then
## began to store an image per folder until it reached 998GOPRO and halted after checking that
## the folder, 999GOPRO already existed.

## This script combines all the images into a single folder.

startFolder = 100   #the first folder in the dir. is 100GOPRO
endFolder = 999     #the last folder in the dir. is 998GOPRO. No typo here 
numImg = 1668      #the first image to be appended to the timelapse dir is in the dir. is GOPR1668.JPG

currentDir="/home/babadines/Documents/Open_House/DCIM_editable"
newDir="/home/babadines/Documents/Open_House/Timelapse/"

for i in range(startFolder, endFolder):
    subFolder = os.path.join(currentDir, str(i) + "GOPRO")
    subFile = os.path.join(subFolder,"GOPR0001.JPG"); # all the subFolders contain a single file named "GOPR0001.JPG"

    newName = "GOPR"+str(numImg)+".JPG"
    newFile = os.path.join(newDir, newName) #prepare to merege files to Timelapse dir

    os.rename(subFile,newFile)

    numImg = numImg + 1


