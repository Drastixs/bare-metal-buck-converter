#include "regions.h"

//region drawer
//public
c_region_drawer::c_region_drawer(uint8_t width, uint8_t height){
    __width = width;
    __height = height;
    __rgn.x = 0;
    __rgn.y = 0;
}

bool c_region_drawer::isFull(){
    return __drawCommandsLen == MAX_STORED_DRAW_COMMANDS;
}

void c_region_drawer::clear(){
    __drawCommandsLen = 0;
}

void c_region_drawer::pixel(pos_t pos){
    draw_command_t drawCommand;
    drawCommand.type = CMD_PIXEL;
    drawCommand.pixel.x = pos.x;
    drawCommand.pixel.y = pos.y;
    __addDrawCommand(drawCommand);
}

void c_region_drawer::line(pos_t startPos, pos_t endPos){
    draw_command_t cmd;
    cmd.type = CMD_LINE;
    cmd.line.x0 = startPos.x;
    cmd.line.y0 = startPos.y;
    cmd.line.x1 = endPos.x;
    cmd.line.y1 = endPos.y;
    // m/c kept for reference only; rasterization uses the endpoints (Bresenham)
    short dy = endPos.y - startPos.y;
    short dx = endPos.x - startPos.x;
    if (dx == 0){
       cmd.line.m = INFINITE_GRADIENT;
       cmd.line.c = startPos.x;
    }
    else {
       cmd.line.m = (float)dy / (float)dx;
       cmd.line.c = startPos.y - cmd.line.m * startPos.x;
    }
    __addDrawCommand(cmd);
}

void c_region_drawer::rect(pos_t startPos, uint8_t width, uint8_t height){
    draw_command_t drawCommand;
    drawCommand.type = CMD_RECT;
    drawCommand.rect.x = startPos.x;
    drawCommand.rect.y = startPos.y;
    drawCommand.rect.w = width;
    drawCommand.rect.h = height;
    __addDrawCommand(drawCommand);
}

void c_region_drawer::circle(pos_t center, uint8_t radius){
    draw_command_t drawCommand;
    drawCommand.type = CMD_CIRCLE;
    drawCommand.circle.x = center.x;
    drawCommand.circle.y = center.y;
    drawCommand.circle.r = radius;
    __addDrawCommand(drawCommand);
}

void c_region_drawer::text(pos_t pos, const char* str){
    draw_command_t drawCommand;
    drawCommand.type = CMD_TEXT;
    drawCommand.text.x = pos.x;
    drawCommand.text.y = pos.y;
    drawCommand.text.str = str;
    __addDrawCommand(drawCommand);
}


//loop through all regions; for each, rasterize only the commands that touch it.

void c_region_drawer::resetLoop(){
    __rgn.x = 0;
    __rgn.y = 0;
}

bool c_region_drawer::hasLoop(){
    return __rgn.y < __height;
}


region_t c_region_drawer::loop(block_t* block_p){
    region_t oldRegion = __rgn;
    if (__rgn.y >= __height){return __rgn;}//return if at end
    draw_command_t cmd;
    for (uint8_t i = 0;i < __drawCommandsLen;i++){
        cmd = __drawCommands[i];
        if (cmd.type == CMD_PIXEL){
            if (isPixelInRegion(__rgn, cmd)){
                drawPixelOnBlock(block_p,cmd);
            }
        }
        else if (cmd.type == CMD_LINE){
            if (isLineInRegion(__rgn,cmd)){
                drawLineOnBlock(block_p,cmd,__rgn);
            }
        }
        else if (cmd.type == CMD_CIRCLE){
            if (isCircleInRegion(__rgn,cmd)){
                drawCircleOnBlock(block_p,cmd,__rgn);
            }
        }
        else if (cmd.type == CMD_RECT){
            if (isRectInRegion(__rgn,cmd)){
                drawRectOnBlock(block_p,cmd,__rgn);
            }
        }
        else if (cmd.type == CMD_TEXT){
            if (isTextInRegion(__rgn,cmd)){
                drawTextOnBlock(block_p,cmd,__rgn);
            }
        }
    }
    __incrementLoopRegion();
    return oldRegion;
}

//private
void c_region_drawer::__addDrawCommand(draw_command_t drawCommand){
    if (isFull()){
        return;
    }
    __drawCommands[__drawCommandsLen] = drawCommand;
    __drawCommandsLen++;
}

void c_region_drawer::__incrementLoopRegion(){
    __rgn.x += REGION_WIDTH;
    if (__rgn.x >= __width){
        __rgn.x = 0;
        __rgn.y += REGION_HEIGHT;   // hasLoop() ends the walk once y >= __height
    }
}
