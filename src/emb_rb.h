#ifndef EMB_RB_H_
#define EMB_RB_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

typedef struct
{
   uint8_t *bP;
   uint32_t size, head, tail;
} emb_rb_t;

uint32_t emb_rb_init(emb_rb_t *rb, uint8_t *bP, uint32_t size);
uint32_t emb_rb_size(emb_rb_t *rb);
uint32_t emb_rb_queue(emb_rb_t *rb, uint8_t *bytes, uint32_t len);
uint32_t emb_rb_dequeue(emb_rb_t *rb, uint8_t *bytes, uint32_t len);
uint32_t emb_rb_peek(emb_rb_t *rb, uint8_t *bytes, uint32_t len);
uint32_t emb_rb_flush(emb_rb_t *rb);
uint32_t emb_rb_free_space(emb_rb_t *rb);
uint32_t emb_rb_used_space(emb_rb_t *rb);

#ifdef __cplusplus
}
#endif

#endif /* EMB_RB_H_ */
