#ifndef ARCH_DIO_H
#define ARCH_DIO_H

#include <config.h>
#include <linux/types.h>

#define SDRAM_BB_SIZE_CH0 CONFIG_SDRAM0_SIZE
#define SDRAM_BB_SIZE_CH1 CONFIG_SDRAM1_SIZE
#define SDRAM_BB_SIZE_CH2 CONFIG_SDRAM2_SIZE

#define SDRAM_BB_BASEADDR_CH0 CONFIG_SDRAM0_BASE
#define SDRAM_BB_BASEADDR_CH1 CONFIG_SDRAM1_BASE
#define SDRAM_BB_BASEADDR_CH2 CONFIG_SDRAM2_BASE

/**
 * DDR���󥿡��ե��������Τ�, DIO��DQ block (16bitñ��)�����Ĥ��뤫����ꤹ��ޥ���.
 * (DQ_BLOCK * 16)��DDR���󥿡��ե�������bit���ˤʤ�.
 */
#if CONFIG_DDR_NUM_CH2 > 0
# define DQ_BLOCKS 4
#else
# define DQ_BLOCKS 3
#endif

#define ROFFSET_POSBASE -2
#define ROFFSET_NEGBASE  2

/**
 * Write Leveling��Ϣ�Υ쥸���������ͤ��Ǽ���빽¤��.
 * dio_get_wl_regs(), dio_set_wl_reg()�ؿ���Ȥä��ɤ߽񤭤���.
 */
struct dio_wl_regs{
    /** CK���Ф���, DQS, DQ, DQM�����ٱ��̤Υ쥸����������. Byteñ��. */
    u8 clock[DQ_BLOCKS][2];
};

/**
 * DQSEN Adjust��Ϣ�Υ쥸���������ͤ��Ǽ���빽¤��.
 * dio_get_dqsen_regs(), dio_set_dqsen_reg()�ؿ���Ȥä��ɤ߽񤭤���.
 */
struct dio_dqsen_regs{
    /** ��Ĵ��(���å��Ǥ�Ĵ��)�Ǥ��ٱ��̤Υ쥸����������. Byteñ��. */
    u8 adj[DQ_BLOCKS][2];
    /** ��Ĵ��(����ǽ�ٱ��ϩ�Ǥ�Ĵ��)�Ǥ��ٱ��̤Υ쥸����������. Byteñ��. */
    u8 ctl[DQ_BLOCKS][2];
};

/**
 * ToffsetĴ����Ϣ�Υ쥸���������ͤ��Ǽ���빽¤��.
 * dio_get_toffset_regs(), dio_set_toffset_reg()�ؿ���Ȥä��ɤ߽񤭤���.
 */
struct dio_toffset_regs{
    /** Write����, DQS���Ф���DQ���ٱ���. ��������å��饤��. Bitñ��. */
    u8 clock[DQ_BLOCKS][16];
    /** Write����, DQS���Ф���DQ���ٱ���. �����ǡ����饤��. �̾�, clock��Ʊ���ͤ����. Bitñ��. */
    u8 data[DQ_BLOCKS][16];
    /** Write����, DQS���Ф���DQM���ٱ���. ��������å��饤��. Byteñ��. */
    u8 mclock[DQ_BLOCKS][2];
    /** Write����, DQS���Ф���DQM���ٱ���. �����ǡ����饤��. �̾�, mclock��Ʊ���ͤ����. Byteñ��. */
    u8 mdata[DQ_BLOCKS][2];
};

/**
 * RoffsetĴ����Ϣ�Υ쥸���������ͤ��Ǽ���빽¤��.
 * dio_get_roffset_regs(), dio_set_roffset_reg()�ؿ���Ȥä��ɤ߽񤭤���.
 */
struct dio_roffset_regs{
    /** Read����, DQ��á��DQS��SoC����������ٱ���. Rizing Edge, Bitñ��. */
    u8 clockp[DQ_BLOCKS][16];
    /** Read����, DQ��á��DQS��SoC����������ٱ���. Falling Edge, Bitñ��. */
    u8 clockn[DQ_BLOCKS][16];
    /** Read����, DQ���ٱ���. �̾��0����. Bitñ��. */
    u8 data[DQ_BLOCKS][16];
};

/**
 * Write Leveling��Ϣ�Υ쥸���������ͤ��������ؿ�.
 * @param wl_regs �쥸����������Ǽ���빽¤�ΤؤΥݥ���.
 * @return None
 */
void dio_get_wl_regs(struct dio_wl_regs *wl_regs);

/**
 * Write Leveling��Ϣ�Υ쥸���������ͤ����ꤹ��ؿ�.
 * @param wl_regs �쥸����������Ǽ���빽¤�ΤؤΥݥ���.
 * @return None
 */
void dio_set_wl_regs(const struct dio_wl_regs *wl_regs);

/**
 * DQSEN Adjust��Ϣ�Υ쥸���������ͤ��������ؿ�.
 * @param dqsen_regs �쥸����������Ǽ���빽¤�ΤؤΥݥ���.
 * @return None
 */
void dio_get_dqsen_regs(struct dio_dqsen_regs *dqsen_regs);

/**
 * DQSEN Adjust��Ϣ�Υ쥸���������ͤ����ꤹ��ؿ�.
 * @param dqsen_regs �쥸����������Ǽ���빽¤�ΤؤΥݥ���.
 * @return None
 */
void dio_set_dqsen_regs(const struct dio_dqsen_regs *dqsen_regs);

/**
 * ToffsetĴ����Ϣ�Υ쥸���������ͤ��������ؿ�.
 * @param toffset_regs �쥸����������Ǽ���빽¤�ΤؤΥݥ���.
 * @return None
 */
void dio_get_toffset_regs(struct dio_toffset_regs *toffset_regs);

/**
 * ToffsetĴ����Ϣ�Υ쥸���������ͤ����ꤹ��ؿ�.
 * @param toffset_regs �쥸����������Ǽ���빽¤�ΤؤΥݥ���.
 * @return None
 */
void dio_set_toffset_regs(const struct dio_toffset_regs *toffset_regs);

/**
 * RoffsetĴ����Ϣ�Υ쥸���������ͤ��������ؿ�.
 * @param roffset_regs �쥸����������Ǽ���빽¤�ΤؤΥݥ���.
 * @return None
 */
void dio_get_roffset_regs(struct dio_roffset_regs *roffset_regs);

/**
 * RoffsetĴ����Ϣ�Υ쥸���������ͤ����ꤹ��ؿ�.
 * @param roffset_regs �쥸����������Ǽ���빽¤�ΤؤΥݥ���.
 * @return None
 */
void dio_set_roffset_regs(const struct dio_roffset_regs *roffset_regs);


/**
 * Write Leveling��������, Ĵ�����, �ܺ٥����Ǽ���빽¤��.
 * ���ι�¤�Τ�ɬ�פ������ͤ������, dio_calib_wl()�ؿ���ƤӽФ����Ȥ�Write Leveling��¹Ԥ���.
 */
struct dio_wl{
    /** Write Leveling��Ĵ����̤������ͤ���礭���ʤä�����, ����Ū��0���֤��������ޤ�.
     �����, CK��DQS����Ӥϰ�����ӤǤ���, Write Leveling��Ĵ���ͤ�1����ʬ��äƤ��ޤ����Ȥ����뤳�Ȥؤ��к��Ǥ�. */
    int  valid_max[DQ_BLOCKS][2];
    /** Write Leveling�Υ�����Ǽ���������. ��Ǿܺ٥ץ�åȤ�ɽ������뤿���OK/NG�����ǡ���. */
    u8 log[DQ_BLOCKS][128];
    /** Write Leveling��Ĵ�����. �쥸�����˥��åȤ���ޤ�. */
    int  edge[DQ_BLOCKS][2];
};

/**
 * DQSEN adjust��������, Ĵ�����, �ܺ٥����Ǽ���빽¤��.
 * ���ι�¤�Τ�ɬ�פ������ͤ������, dio_calib_dqsen()�ؿ���ƤӽФ����Ȥ�DQSEN Adjust��¹Ԥ���.
 */
struct dio_dqsen{
    /** Ĵ��������. ��Ĵ��(adj)�Ǥ��ٱ���. */
    int range_min;
    /** 1�ս�θ������Ф�, ��Ӥ򲿲󷫤��֤���. �����¿�������٤Ͼ夬�뤬�����֤�������. */
    int iteration;
    /** �ǡ�����Ӥ�Ȥ���. true�ʤ�DQS��rising edge�ο��ȥǡ���write/read/compare��Ƚ�ꤷ, DQS��rising edge�ο�������Ƚ�ꤷ�ޤ�. */
    int use_data_compare;
    /** DQSEN adjust��Ĵ����̤Υ��󥿡��� (64 * adj + ctl). */
    int width[DQ_BLOCKS][2];
    /** DQSEN adjust��Ĵ����̤��� (64 * adj + ctl). */
    int center[DQ_BLOCKS][2];
};

/**
 * ToffsetĴ����������, Ĵ�����, �ܺ٥����Ǽ���빽¤��.
 * ���ι�¤�Τ�ɬ�פ������ͤ������, dio_calib_toffset()�ؿ���ƤӽФ����Ȥ�ToffsetĴ����¹Ԥ���.
 */
struct dio_toffset{
    /** Ĵ��������. DQ, DQM����. */
    int   range_min;
    /** Ĵ����λ��. DQ, DQM����. */
    int   range_max;
    /** ��û�Τ���, �����ʬ�θ����򥹥��åפ��뤫�ɤ����Υե饰. */
    int  skip_flag;
    /** �����ͤ��饹���åפ���. skip_flag �� false�ʤ�̵�뤵���. */
    int   skip_min;
    /** �����ͤޤǥ����åפ���. skip_flag �� false�ʤ�̵�뤵���. */
    int   skip_max;
    /** ToffsetĴ���˻��Ѥ���ǡ�����ӥѥ�����. */
    const u32 *pattern_data;
    /** ToffsetĴ���˻��Ѥ���ǡ�����ӥѥ������. */
    int   pattern_length;
    /** 1�ս�θ������Ф�, ��Ӥ򲿲󷫤��֤���. �����¿�������٤Ͼ夬�뤬�����֤�������. */
    int   iteration;
    /** Toffset�Υ�����Ǽ���������. ��Ǿܺ٥ץ�åȤ�ɽ������뤿���DQ��OK/NG�����ǡ���. */
    u16 log[DQ_BLOCKS][128];
    /** ToffsetĴ����̤���Ǽ���������. DQ����. DQ_BLOCKS������16bitʬ. */
    int   width[DQ_BLOCKS][16];
    /** ToffsetĴ����̤���Ǽ���������. DQ�Υ��󥿡���. DQ_BLOCKS������16bitʬ. */
    int   center[DQ_BLOCKS][16];
    /** Toffset�Υ�����Ǽ���������. ��Ǿܺ٥ץ�åȤ�ɽ������뤿���DQM��OK/NG�����ǡ���. */
    u16 mlog[DQ_BLOCKS][128];
    /** ToffsetĴ����̤���Ǽ���������. DQM����. DQ_BLOCKS������2bitʬ. */
    int   mwidth[DQ_BLOCKS][2];
    /** ToffsetĴ����̤���Ǽ���������. DQM�Υ��󥿡���. DQ_BLOCKS������2bitʬ. */
    int   mcenter[DQ_BLOCKS][2];
};

/**
 * RoffsetĴ����������, Ĵ�����, �ܺ٥����Ǽ���빽¤��.
 * ���ι�¤�Τ�ɬ�פ������ͤ������, dio_calib_roffset()�ؿ���ƤӽФ����Ȥ�RoffsetĴ����¹Ԥ���.
 */
struct dio_roffset{
    /** Ĵ��������. DQ��á��DQS��SoC����������ٱ���. */
    int   range_min;
    /** Ĵ����λ��. DQ��á��DQS��SoC����������ٱ���. */
    int   range_max;
    /** Roffset���DQ�����ɤ�����٤餻�뤫. range_min, range_max�򾮤��������˥��եȤ�����Τ�Ʊ���θ��̤�����.
        Ĵ����������ޥ��ʥ�����Ϥ᤿�����˼�˻��Ѥ���. */
    int   data_delay;
    /** ��û�Τ���, �����ʬ�θ����򥹥��åפ��뤫�ɤ����Υե饰. */
    int  skip_flag;
    /** �����ͤ��饹���åפ���. skip_flag �� false�ʤ�̵�뤵���. */
    int   skip_min;
    /** �����ͤޤǥ����åפ���. skip_flag �� false�ʤ�̵�뤵���. */
    int   skip_max;
    /** RoffsetĴ���˻��Ѥ���ǡ�����ӥѥ�����. */
    const u32 *pattern_data;
    /** RoffsetĴ���˻��Ѥ���ǡ�����ӥѥ������. */
    int   pattern_length;
    /** 1�ս�θ������Ф�, ��Ӥ򲿲󷫤��֤���. �����¿�������٤Ͼ夬�뤬�����֤�������. */
    int   iteration;
    /** Roffset�Υ�����Ǽ���������. ��Ǿܺ٥ץ�åȤ�ɽ������뤿���OK/NG�����ǡ���. */
    u32 log[DQ_BLOCKS][128];
    /** RoffsetĴ����̤���Ǽ���������. ��. DQ_BLOCKS������Postive Edge16bit + Neggative Edge16bit��32bitʬ. */
    int   width[DQ_BLOCKS][32];
    /** RoffsetĴ����̤���Ǽ���������. ���󥿡���. DQ_BLOCKS������Postive Edge16bit + Neggative Edge16bit��32bitʬ. */
    int   center[DQ_BLOCKS][32];
    /** RoffsetĴ����̤���Ǽ���������. �Ǿ���. DQ_BLOCKS������Postive Edge16bit + Neggative Edge16bit��32bitʬ. */
    int   min[DQ_BLOCKS][32];
    /** RoffsetĴ����̤���Ǽ���������. ������. DQ_BLOCKS������Postive Edge16bit + Neggative Edge16bit��32bitʬ. */
    int   max[DQ_BLOCKS][32];
};

/**
 * Write Leveling��¹Ԥ���ؿ�.
 * Write Leveling�Ȥ�CK��DQS�ΰ��꺹���ä���Ĵ�������Ǥ���.
 * @param wl Write Leveling��������, Ĵ�����, �ܺ٥����Ǽ���빽¤�ΤؤΥݥ���
 * @return None
 */
void dio_calib_wl(struct dio_wl *wl);

/**
 * DQSEN Adjust��¹Ԥ���ؿ�.
 * DQSEN Adjust�Ȥ�DDR���꤬�֤��Ƥ���DQS����ˤ�����ޥ�����ͭ���ϰϤ���빩���Ǥ���.
 * @param dqsen DQSEN adjust��������, Ĵ�����, �ܺ٥����Ǽ���빽¤�ΤؤΥݥ���
 * @return None
 */
void dio_calib_dqsen(struct dio_dqsen *dqsen);

/**
 * ToffsetĴ����¹Ԥ���ؿ�.
 * Toffset�Ȥ�, DDR�����Write����Ȥ���DQS��DQ�饤��ΰ��꺹�Τ��ȤǤ���.
 * @param toffset ToffsetĴ����������, Ĵ�����, �ܺ٥����Ǽ���빽¤�ΤؤΥݥ���
 * @return None
 */
void dio_calib_toffset(struct dio_toffset *toffset);

/**
 * RoffsetĴ����¹Ԥ���ؿ�.
 * Roffset�Ȥ�, DDR���꤫��Read����Ȥ���DQS��DQ�饤��ΰ��꺹�Τ��ȤǤ���.
 * @param roffset RoffsetĴ����������, Ĵ�����, �ܺ٥����Ǽ���빽¤�ΤؤΥݥ���
 * @return None
 */
void dio_calib_roffset(struct dio_roffset *roffset);

//#define DIO_BUS32H 3
//#define DIO_BUS32L 2
#define DIO_BUS32  2
#define DIO_BUS16  0

void  dio_datacpy(u32 addr, const u32 *data ,int length, int bus_mode);
u32 dio_datacmp(u32 addr, const u32 *data ,int length, int bus_mode);

/**
 * Toffset/RoffsetĴ���˻��Ѥ���ǡ�����ӥѥ�����.
 */
extern const u32 dio_pattern_data[];

/**
 * Toffset/RoffsetĴ���˻��Ѥ���ǡ�����ӥѥ������.
 * 1�ѥ����� = 16byte �ʤΤ�, �Х��ȿ��Ǥ����� (dio_pattern_length * 16)byte�ˤʤ�.
 */
extern const int   dio_pattern_length;

#endif /* ARCH_DIO_H */
