copy /y CardFace.pro CardFace.pro_bak
..\public\sed 's/utf8//g' CardFace.pro >CardFace.txt
copy /y CardFace.txt CardFace.pro
del CardFace.txt
qmake -r -spec win32-msvc2013 -tp vc 
copy CardFace.pro_bak CardFace.pro
del CardFace.pro_bak