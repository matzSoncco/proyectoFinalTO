#include <QApplication>
#include "../include/VentanaPrincipal.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Configurar estilo de la aplicación
    app.setStyle("Fusion");
    
    // Crear y mostrar la ventana principal
    VentanaPrincipal ventana;
    ventana.show();
    
    // El bucle principal de Qt
    return app.exec();
}