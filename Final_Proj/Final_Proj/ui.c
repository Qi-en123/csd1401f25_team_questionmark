//#include "cprocessing.h"
//
//void watering(pot* pot, float dt) {
//    // Progress bar settings
//    float rectWidth = 60;
//    float rectHeight = 30;
//    int potBarSegments = 5;
//    float spacing = 0.0f;
//    float fillSpeed = 0.4f;
//
//    // Position of the bar
//    float rectX = pot->position.x - rectWidth / 2.0f;
//    float rectY = pot->position.y - (120.0f / 2.0f) - rectHeight - 10.0f;
//
//    CP_Settings_RectMode(CP_POSITION_CORNER);
//
//    // Update fill progress
//    pot->fillProgress += fillSpeed * dt;
//    if (pot->fillProgress > 1.0f) pot->fillProgress = 1.0f;
//
//    // Lock watering if fully filled
//    if (pot->fillProgress >= 1.0f) {
//        pot->wateredOnce = true;
//        pot->watered = true;
//    }
//
//    // Draw progress bar segments
//    float totalBarWidth = potBarSegments * rectWidth + (potBarSegments - 1) * spacing;
//    float filledWidth = totalBarWidth * pot->fillProgress;
//    float thresholdWidth = (potBarSegments - 2) * rectWidth;
//
//    for (int seg = 0; seg < potBarSegments; seg++) {
//        float x = rectX + seg * (rectWidth + spacing) - 82;
//
//        // Base segment color
//        if (seg == potBarSegments - 2) {
//            CP_Settings_Fill(CP_Color_Create(255, 150, 150, 255));
//        }
//        else {
//            CP_Settings_Fill(CP_Color_Create(120, 120, 120, 255));
//        }
//        CP_Graphics_DrawRect(x, rectY, rectWidth, rectHeight);
//
//        // Draw filled part
//        float segmentStart = seg * (rectWidth + spacing);
//        float segmentFill = filledWidth - segmentStart;
//        if (segmentFill > 0.0f) {
//            if (segmentFill > rectWidth) segmentFill = rectWidth;
//
//            CP_Color fillColor;
//            if (filledWidth <= thresholdWidth) {
//                fillColor = CP_Color_Create(0, 0, 255, 255); // blue
//            }
//            else {
//                float redAmount = (filledWidth - thresholdWidth) / (totalBarWidth - thresholdWidth);
//                if (redAmount > 1.0f) redAmount = 1.0f;
//                int r = (int)(255.0f * redAmount);
//                int g = 0;
//                int b = (int)(255.0f * (1.0f - redAmount));
//                fillColor = CP_Color_Create(r, g, b, 255);
//            }
//
//            CP_Settings_Fill(fillColor);
//            CP_Graphics_DrawRect(x, rectY, segmentFill, rectHeight);
//        }
//
//        // Optional top yellow overlay
//        CP_Settings_Fill(CP_Color_Create(255, 255, 0, 255));
//        CP_Graphics_DrawRect(x, rectY - rectHeight - 8, rectWidth, rectHeight);
//	}
//}