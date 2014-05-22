/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: student.c 337 2014-02-25 06:52:49Z spanel $
 */

#include "student.h"
#include "transform.h"

#include <memory.h>
#include <stdbool.h>

/******************************************************************************
 * Globalni promenne a konstanty
 */

/* rozmer textury */
const int       TEXTURE_SIZE    = 512;

/* pocet policek sachovnice */
const int       NUM_OF_TILES    = 16;

/* barva poli */
const S_RGBA    BLACK_TILE      = { 75, 75, 75 };
const S_RGBA    WHITE_TILE      = { 255, 255, 255 };


/*****************************************************************************
 * Funkce vytvori vas renderer a nainicializuje jej
 */

S_Renderer * studrenCreate()
{
    int i;
    bool light = true;

    S_StudentRenderer * renderer =
        (S_StudentRenderer *)malloc(sizeof(S_StudentRenderer));
    IZG_CHECK(renderer, "Cannot allocate enough memory");

    /* inicializace default rendereru */
    renInit(&renderer->base);

    /* nastaveni ukazatelu na upravene funkce */
    renderer->base.releaseFunc = studrenRelease;
    renderer->base.projectTriangleFunc= studrenProjectTriangle;

    /* inicializace nove pridanych casti */
    renderer->texture = (S_RGBA*) malloc(sizeof(S_RGBA) * TEXTURE_SIZE * TEXTURE_SIZE);
    IZG_CHECK(renderer->texture, "Cannot allocate enough memory");

    for (i = 0; i < (TEXTURE_SIZE * TEXTURE_SIZE); i++) {
        if (((((NUM_OF_TILES * NUM_OF_TILES * 32) - i) % 32) == 0) 
        && ((i % (TEXTURE_SIZE * (TEXTURE_SIZE / NUM_OF_TILES)) != 0))) {
            light = (!light);
        }

        (light) ?
            (renderer->texture[i] = WHITE_TILE) :
            (renderer->texture[i] = BLACK_TILE) ;
    }

    //printf("Kontrola textury: %d", saveBitmap("f.bmp", renderer->texture,
    //            TEXTURE_SIZE, TEXTURE_SIZE));

    return (S_Renderer *)renderer;
}

/*****************************************************************************
 * Funkce korektne zrusi renderer a uvolni pamet
 */

void studrenRelease(S_Renderer **ppRenderer)
{
    S_StudentRenderer * renderer;

    if( ppRenderer && *ppRenderer )
    {
        renderer = (S_StudentRenderer *)(*ppRenderer);

        /* pripadne uvolneni pameti */
        free(renderer->texture);

        /* fce default rendereru */
        renRelease(ppRenderer);
    }
}

/******************************************************************************
 * Nova fce pro rasterizaci trojuhelniku do frame bufferu
 * s podporou texturovani a interpolaci texturovacich souøadnic
 * Pozn.: neni nutné øe¹it perspektivní korekci textury
 * v1, v2, v3 - ukazatele na vrcholy trojuhelniku ve 3D pred projekci
 * n1, n2, n3 - ukazatele na normaly ve vrcholech ve 3D pred projekci
 * t1, t2, t3 - ukazatele na texturovaci souradnice vrcholu
 * x1, y1, ... - vrcholy trojuhelniku po projekci do roviny obrazovky
 */

void studrenDrawTriangle(S_Renderer *pRenderer,
                         S_Coords *v1, S_Coords *v2, S_Coords *v3,
                         S_Coords *n1, S_Coords *n2, S_Coords *n3,
                         S_Coords *t1, S_Coords *t2, S_Coords *t3,
                         int x1, int y1,
                         int x2, int y2,
                         int x3, int y3
                         )
{
    int         minx, miny, maxx, maxy;
    int         a1, a2, a3, b1, b2, b3, c1, c2, c3;
    int         s1, s2, s3;
    int         x, y, e1, e2, e3;
    double      alpha, beta, w1, w2, w3, z;
    double      u, v;
    S_RGBA      col1, col2, col3, color;
    S_RGBA      tmpColor;

    IZG_ASSERT(pRenderer && v1 && v2 && v3 && n1 && n2 && n3);

    /* vypocet barev ve vrcholech */
    col1 = pRenderer->calcReflectanceFunc(pRenderer, v1, n1);
    col2 = pRenderer->calcReflectanceFunc(pRenderer, v2, n2);
    col3 = pRenderer->calcReflectanceFunc(pRenderer, v3, n3);

    /* obalka trojuhleniku */
    minx = MIN(x1, MIN(x2, x3));
    maxx = MAX(x1, MAX(x2, x3));
    miny = MIN(y1, MIN(y2, y3));
    maxy = MAX(y1, MAX(y2, y3));

    /* oriznuti podle rozmeru okna */
    miny = MAX(miny, 0);
    maxy = MIN(maxy, pRenderer->frame_h - 1);
    minx = MAX(minx, 0);
    maxx = MIN(maxx, pRenderer->frame_w - 1);

    /* Pineduv alg. rasterizace troj.
       hranova fce je obecna rovnice primky Ax + By + C = 0
       primku prochazejici body (x1, y1) a (x2, y2) urcime jako
       (y1 - y2)x + (x2 - x1)y + x1y2 - x2y1 = 0 */

    /* normala primek - vektor kolmy k vektoru mezi dvema vrcholy, tedy (-dy, dx) */
    a1 = y1 - y2;
    a2 = y2 - y3;
    a3 = y3 - y1;
    b1 = x2 - x1;
    b2 = x3 - x2;
    b3 = x1 - x3;

    /* koeficient C */
    c1 = x1 * y2 - x2 * y1;
    c2 = x2 * y3 - x3 * y2;
    c3 = x3 * y1 - x1 * y3;

    /* vypocet hranove fce (vzdalenost od primky) pro protejsi body */
    s1 = a1 * x3 + b1 * y3 + c1;
    s2 = a2 * x1 + b2 * y1 + c2;
    s3 = a3 * x2 + b3 * y2 + c3;

    /* normalizace, aby vzdalenost od primky byla kladna uvnitr trojuhelniku */
    if( s1 < 0 )
    {
        a1 *= -1;
        b1 *= -1;
        c1 *= -1;
    }
    if( s2 < 0 )
    {
        a2 *= -1;
        b2 *= -1;
        c2 *= -1;
    }
    if( s3 < 0 )
    {
        a3 *= -1;
        b3 *= -1;
        c3 *= -1;
    }

    /* koeficienty pro barycentricke souradnice */
    alpha = 1.0 / ABS(s2);
    beta = 1.0 / ABS(s3);
    /*gamma = 1.0 / ABS(s1);*/

    /* vyplnovani... */
    for( y = miny; y <= maxy; ++y )
    {
        /* inicilizace hranove fce v bode (minx, y) */
        e1 = a1 * minx + b1 * y + c1;
        e2 = a2 * minx + b2 * y + c2;
        e3 = a3 * minx + b3 * y + c3;

        for( x = minx; x <= maxx; ++x )
        {
            if( e1 >= 0 && e2 >= 0 && e3 >= 0 )
            {
                /* interpolace pomoci barycentrickych souradnic
                   e1, e2, e3 je aktualni vzdalenost bodu (x, y) od primek */
                w1 = alpha * e2;
                w2 = beta * e3;
                w3 = 1.0 - w1 - w2;

                /* interpolace z-souradnice */
                u = w1 * t1->x + w2 * t2->x + w3 * t3->x;
                v = w1 * t1->y + w2 * t2->y + w3 * t3->y;
                z = w1 * v1->z + w2 * v2->z + w3 * v3->z;

                /* interpolace barvy */
                color.red = ROUND2BYTE(w1 * col1.red + w2 * col2.red + w3 * col3.red);
                color.green = ROUND2BYTE(w1 * col1.green + w2 * col2.green + w3 * col3.green);
                color.blue = ROUND2BYTE(w1 * col1.blue + w2 * col2.blue + w3 * col3.blue);
                color.alpha = 255;

                // zjisteni barvy z pRenderer na souradnicich [u|v]
                tmpColor = studrenTextureValue((S_StudentRenderer *) pRenderer, u, v);

                color.red = (color.red * tmpColor.red) / color.alpha;
                color.green = (color.green * tmpColor.green) / color.alpha;
                color.blue = (color.blue * tmpColor.blue) / color.alpha;

                /* vykresleni bodu */
                if( z < DEPTH(pRenderer, x, y) )
                {
                    PIXEL(pRenderer, x, y) = color;
                    DEPTH(pRenderer, x, y) = z;
                }
            }

            /* hranova fce o pixel vedle */
            e1 += a1;
            e2 += a2;
            e3 += a3;
        }
    }
}

/******************************************************************************
 * Vykresli i-ty trojuhelnik modelu pomoci nove fce studrenDrawTriangle()
 * Pred vykreslenim aplikuje na vrcholy a normaly trojuhelniku
 * aktualne nastavene transformacni matice!
 * i - index trojuhelniku
 */

void studrenProjectTriangle(S_Renderer *pRenderer, S_Model *pModel, int i)
{
    S_Coords    aa, bb, cc;             /* souradnice vrcholu po transformaci */
    S_Coords    naa, nbb, ncc;          /* normaly ve vrcholech po transformaci */
    S_Coords    nn;                     /* normala trojuhelniku po transformaci */
    int         u1, v1, u2, v2, u3, v3; /* souradnice vrcholu po projekci do roviny obrazovky */
    S_Triangle  * triangle;

    IZG_ASSERT(pRenderer && pModel && i >= 0 && i < trivecSize(pModel->triangles));

    /* z modelu si vytahneme trojuhelnik */
    triangle = trivecGetPtr(pModel->triangles, i);

    /* transformace vrcholu matici model */
    trTransformVertex(&aa, cvecGetPtr(pModel->vertices, triangle->v[0]));
    trTransformVertex(&bb, cvecGetPtr(pModel->vertices, triangle->v[1]));
    trTransformVertex(&cc, cvecGetPtr(pModel->vertices, triangle->v[2]));

    /* promitneme vrcholy trojuhelniku na obrazovku */
    trProjectVertex(&u1, &v1, &aa);
    trProjectVertex(&u2, &v2, &bb);
    trProjectVertex(&u3, &v3, &cc);

    /* pro osvetlovaci model transformujeme take normaly ve vrcholech */
    trTransformVector(&naa, cvecGetPtr(pModel->normals, triangle->v[0]));
    trTransformVector(&nbb, cvecGetPtr(pModel->normals, triangle->v[1]));
    trTransformVector(&ncc, cvecGetPtr(pModel->normals, triangle->v[2]));

    /* normalizace normal */
    coordsNormalize(&naa);
    coordsNormalize(&nbb);
    coordsNormalize(&ncc);

    /* transformace normaly trojuhelniku matici model */
    trTransformVector(&nn, cvecGetPtr(pModel->trinormals, triangle->n));

    /* normalizace normaly */
    coordsNormalize(&nn);

    /* je troj. privraceny ke kamere, tudiz viditelny? */
    if( !renCalcVisibility(pRenderer, &aa, &nn) )
    {
        /* odvracene troj. vubec nekreslime */
        return;
    }

    /* rasterizace trojuhelniku */
    studrenDrawTriangle(pRenderer,
                    &aa, &bb, &cc,
                    &naa, &nbb, &ncc,
                    // texturovaci souradnice vrchlou
                    vecGetPtr(pModel->texcoords, triangle->v[0]),
                    vecGetPtr(pModel->texcoords, triangle->v[1]),
                    vecGetPtr(pModel->texcoords, triangle->v[2]),
                    //cvecGetPtr(pModel->texcoords, triangle->v[0]),
                    //cvecGetPtr(pModel->texcoords, triangle->v[1]),
                    //cvecGetPtr(pModel->texcoords, triangle->v[2]),
                    u1, v1, u2, v2, u3, v3
    );
}

/******************************************************************************
 * Vrací hodnotu v aktuálnì nastavené textuøe na zadaných
 * texturovacích souøadnicích u, v
 * Pro urèení hodnoty pou¾ívá bilineární interpolaci
 * u, v - texturovací souøadnice v intervalu 0..1, který odpovídá ¹íøce/vý¹ce textury
 */

S_RGBA studrenTextureValue(S_StudentRenderer * pRenderer, double u, double v)
{
    // http://cs.wikipedia.org/wiki/Biline%C3%A1rn%C3%AD_interpolace
    // floor = zaokrouhleni (mozna? int -> incompatible?)
    // x = u; x1 = (int) u; x2 = (int) u+1
    // y = u; y1 = (int) u; y2 = (int) u+1
    //
    // mapovaci funkce : index = u + v * TEXTURE_SIZE
    //
    double r1, r2, r, g, b;
    int x, y, x1, x2, y1, y2;

    if (isnan(u) || isnan(v)) {
        return makeColor(0, 0, 0);
    }

    x = u;
    y = v;
    x1 = (int) (u * TEXTURE_SIZE);
    y1 = (int) (v * TEXTURE_SIZE);
    x2 = (int) (u * TEXTURE_SIZE + 1);
    y2 = (int) (v * TEXTURE_SIZE + 1);


    // RED
    r1 = (x2 - x) / (x2 - x1) * pRenderer->texture[TEXTURE_SIZE * x1 + y1].red + (x - x1) / (x2 - x1) * pRenderer->texture[TEXTURE_SIZE * x2 + y1].red;
    r2 = (x2 - x) / (x2 - x1) * pRenderer->texture[TEXTURE_SIZE * x1 + y2].red + (x - x1) / (x2 - x1) * pRenderer->texture[TEXTURE_SIZE * x2 + y2].red;
    r  = (y2 - y) / (y2 - y1) * r1      + (y - y1) / (y2 - y1) * r2;

    // GREEN
    r1 = (x2 - x) / (x2 - x1) * pRenderer->texture[TEXTURE_SIZE * x1 + y1].green + (x - x1) / (x2 - x1) * pRenderer->texture[TEXTURE_SIZE * x2 + y1].green;
    r2 = (x2 - x) / (x2 - x1) * pRenderer->texture[TEXTURE_SIZE * x1 + y2].green + (x - x1) / (x2 - x1) * pRenderer->texture[TEXTURE_SIZE * x2 + y2].green;
    g  = (y2 - y) / (y2 - y1) * r1      + (y - y1) / (y2 - y1) * r2;

    // BLUE
    r1 = (x2 - x) / (x2 - x1) * pRenderer->texture[TEXTURE_SIZE * x1 + y1].blue + (x - x1) / (x2 - x1) * pRenderer->texture[TEXTURE_SIZE * x2 + y1].blue;
    r2 = (x2 - x) / (x2 - x1) * pRenderer->texture[TEXTURE_SIZE * x1 + y2].blue + (x - x1) / (x2 - x1) * pRenderer->texture[TEXTURE_SIZE * x2 + y2].blue;
    b  = (y2 - y) / (y2 - y1) * r1      + (y - y1) / (y2 - y1) * r2;
   
    return makeColor((char)r, (char)g, (char)b);
       

    //return makeColor(100, 75, 50);
}


/******************************************************************************
 ******************************************************************************
 * Callback funkce volana pri startu aplikace
 * Doplnte automaticke vygenerovani a prirazeni texturovacich souradnic
 * vrcholum modelu s vyuzitim mapovani na kouli
 * Muzete predpokladat, ze model je umisten v pocatku souradneho systemu
 * a posunuti neni treba resit
 */

void onInit(S_Renderer *pRenderer, S_Model *pModel)
{
    // vertices = vektor vsech vrcholu
    S_Coords *verticesPtr;
    S_Coords tmp;
    int i;

    vecInit(pModel->texcoords, sizeof(S_Coords));

    for (i = 0; (i < pModel->vertices->size); i++) {
        verticesPtr  = (S_Coords *) vecGetPtr(pModel->vertices, i);
        tmp = makeCoords(verticesPtr->x, verticesPtr->y, verticesPtr->z);

        coordsNormalize(&tmp);

        tmp.x = 0.5 + (atan2(tmp.z, tmp.x) / (2 * M_PI));
        tmp.y = 0.5 - (asin(tmp.y) / M_PI);
        tmp.z = 0;
        
        vecPushBack(pModel->texcoords, &tmp);

    }
}


/*****************************************************************************
 *****************************************************************************/
