#include <raylib.h>
#include <raymath.h>

static float match(float* l, float* r, int samples) {
    float match = 0;

    for (int i = 0; i < samples; i++) {
        match += l[i] * r[i];
    }

    return 2 * match / samples;
}

static void matched_filter(float* l, float* r, int samples, float* out) {
    for (int i = 0; i < samples; i++) {
        out[i] = (l[i] + r[i]) / 2;
    }
}

float fract(float x) {
    return x - floorf(x);
}

float noise(float t) {
    return 1 - 2 * fract(sin(t * 12.9867f) * 43758.5324f);
}

int main() {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(800, 800, "Matched Filter");
    SetTargetFPS(60);

    float speed = 0.005f;

    float min = 1;
    float max = 0;
    while (true) {
        if (IsKeyPressed(KEY_ESCAPE)) break;

        speed = expf(logf(speed) + ((float) GetMouseWheelMove() * 0.2f));

        if (speed > 3.0f) speed = 3.0f;
        else if (speed < 0.001f) speed = 0.001f;

        float w = GetRenderWidth();
        float h = GetRenderHeight();

        static float t = 0;
        if (!IsKeyDown(KEY_SPACE)) {
            t += GetFrameTime() * speed;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawLine(0, h/2, w, h/2, LIGHTGRAY); // horizontal middle line
            DrawLine(w/2, 0, w/2, h, LIGHTGRAY); // vertical middle line

            DrawText("0", w/2, h/2, 18, LIGHTGRAY); // graph 0 (xy)
            DrawText("pi * -t", 2, h/2, 18, LIGHTGRAY); // graph -x
            DrawText("pi * t", -2 + w - MeasureText("pi * t", 18), h/2, 18, LIGHTGRAY); // graph x
            DrawText("1", w/2, 2, 18, LIGHTGRAY); // graph y (1.0)
            DrawText("-1", w/2, h - 18, 18, LIGHTGRAY); // graph -y (-1.0)

            #define LEN 0x320

            static float l[LEN];
            static float r[LEN];
            static float n[LEN];
            for (int i = 0; i < LEN; i++) {
                float curr = (float) i / LEN - 1;

                // create noise signal
                float noisy = 0.1 * noise(t + i);
                n[i] = fmin(1.0f, r[i] + noisy);

                // simulate two signals
                l[i] = sinf(2*PI * (t + curr));
                r[i] = sinf(2*PI * (t + curr) + t*PI); // moving signal

                float x = w * (float) i / LEN - 1.0f;

                DrawLine(
                    x, h/2 * (1.0f + noisy),
                    x, h/2 * (1.0f - noisy),
                    ColorAlpha(noisy > 0 ? GREEN : RED, 0.3));

                DrawCircle(x, h/2 * (1.0f + l[i]), 2, BLUE);
                DrawCircle(x, h/2 * (1.0f + r[i]), 2, RED);
                DrawCircle(x, h/2 * (1.0f + n[i]), 2, LIME);
            }

            float m = match(l, r, LEN);
            float mn = match(l, n, LEN);
            static float best = -1;
            static float noised = -1;

            if ((int) (GetTime() * 100) % 50 > 45) {
                best = -1;
                noised = -1;
            }

            best = fmax(best, m);
            noised = fmax(best, mn);

            DrawText(TextFormat("Time: %.2fs (%.3fx)", t, speed), w/2, h/2+32, 32, BLACK);
            DrawText(TextFormat("Match: %.2f%%", m * 100), w/2, h/2+64, 32, BLACK);
            DrawText(TextFormat("Best Match: %.2f%%", best * 100), w/2, h/2+96, 32, BLACK);
            DrawText(TextFormat("Noise Match: %.2f%%", noised * 100), w/2, h/2+128, 32, BLACK);

            DrawFPS(8, 8);
        EndDrawing();
    }

    CloseWindow();
}
