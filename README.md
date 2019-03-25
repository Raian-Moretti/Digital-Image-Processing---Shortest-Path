# Digital-Image-Processing---Shortest-Path
This project consists of processing some images which simulate a labyrinth to find the shortest path (left to right). When it finds the right path, it paints a red line on the image.

How images were made:
  
   The images were made with a white paper and the lines with a black pen. However, if you want to test it with your own images, you can use any color, but its important to use a white paper. If the paper's color and the pen's color are too close (on grayscale for ex.: Paper: 220, Pen: 200) the program maybe will find an error when binarizes it. After taking the photo, it's important to  use the right size too. Length 512 to 1024 pixels and height between 256 e 768 pixels. Also important to clarify that images are arrays so if the image has more than 5mb, the program will run through some problems. You can also use synthetic image.
 
How main functions works:
  
  Main functions are responsible for opening the image, creating and saving a matrix image's copy, fill it with image's data and test the path we developed. 
  
 How process functions works:
 
  Process functions are responsible for processing the image with filters (normalize, binarize and filter itself based on a windows which we used a threshold and pixels' distance for define what's black(0) and what's white(255).
  For filtering the imagem we use "filtro" wich runs all over the image, pixel-to-pixel. The fist step is make a window which update the variables looking for the min and max values, or as we see, looking for the lightest and darkest pixels on the window. For the second step we verify if the lightest value is at least higher than the darkest+35, if so we get the threshold value, ((max+min)/2)+15 (we run a lot of test untill get this result). Now that we have the threshold we get to the third step which is trivial, for each data(pixel) in the window higher than the threshold we assume that it's white(255), if it's smaller we set as black(0). If the min and max values are too close we assume everything as white. When the window is done, we move it one pixel and repeat all the steps until we have binarize all the image. As you guys will see, this process will generate the same image you put in, but now binarized in the output.
  
  "caminhos" function is responsible for find the shortest path (left to right) in the already binarized images, we divide it in 6 steps:
  
    1 - Create a matrix where the black pixels will the set as '0' so the algorithm can find it and calculate distance of the beginning.
    2 - Find the beginning (left side) of the image and set it as '1'.
    3 - Replace the '0' of step 1 with the distance of the beginning ('1'), which we find in the step 2.
    4 - Find the ending (right side) of the image.
    5 - Create an array of paths which will be return by our function finding the shortest path (it's a looping for the lesser    numbers we find in the step 3.
    6 - Return the 5 steps' array's size and himself inverted.
    
  When it runs all the steps you'll have an image's copy with a red line on the shortest path. It takes some time to process all the images, so be patient. If it takes too long check ur image's resolution and size (as I said if it's too height, too width or too big - 25MB as example - it won't work). If the binarized image has a failure line you'll need to improve ur image's line, because as you can imagine if you do white lines on a white paper, it also won't work. 
  

CREDITS: 

   Bogdan T. Nassu teacher who developed and shared the files with his students to solve the shortest path problem. 
  
   Leyza B. Dorini and Myriam R. Delgado who were responsibles for guiding us during the project.
  
   Special credits to José Henrique Ivanchechen (https://github.com/joseivanchechen) and Gabriel A. Fanha which developed the project with me.
