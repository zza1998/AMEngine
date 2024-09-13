#ifndef ADFILEBROWSERWIDGET_H
#define ADFILEBROWSERWIDGET_H

#include "Gui/Widget/AdFileBrowserWidget.h"

namespace ade{
    class AdEditorContentWindow{
    public:
        void Draw(bool *pOpen);
    private:
        AdFileBrowserWidget mFileBrowserWidget;
    };
}
#endif