#ifndef MAPEDITORUPDATE_H
#define MAPEDITORUPDATE_H

void editorUpdateDeleteSelection(Map *map, Selection **sel)
{
    assertExpr(map && sel);
    if(!keyPressed(SC_DELETE))
        return;
    while(*sel){
        pieceDelete(map, (*sel)->fields.piece);
        *sel = selFree(*sel);
    }
}

#endif /* end of include guard: MAPEDITORUPDATE_H */
