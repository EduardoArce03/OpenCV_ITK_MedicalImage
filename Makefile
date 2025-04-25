all:
	g++ main.cpp \
      -I/home/andres/aplicaciones/Librerias/opencv/opencvi/include/opencv5 \
      -I/usr/local/include/opencv4 \
      -I/home/andres/ITKBridgeOpenCVExternal/BridgeOpenCV/include \
      -I/usr/local/include/ITK-6.0 \
      -I/home/andres/CLionProjects/Visio-Reso_Magnetica/include \
      -L/home/andres/aplicaciones/Librerias/opencv/opencvi/lib \
      -L/usr/local/lib \
      -litkCommon -litkIOImageBase -litkIOImage -litkImageFilter -litkImaging \
      -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_video -lopencv_videoio \
      -o vison.bin


saludar:
	echo "Compilando el Poryecto de Visio...."

run:
	./vison.bi