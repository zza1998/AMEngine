#ifndef ADEDITORLAYERWINDOW_H
#define ADEDITORLAYERWINDOW_H

namespace ade{
    class AdNode;

    class AdEditorLayerWindow{
    public:
        void Draw(bool *pOpen);
    private:
        void DrawNodeTree(AdNode *node, AdNode **selectedNode);
        void DrawNodeTreeButtons(AdNode* node);
    };
}
#endif