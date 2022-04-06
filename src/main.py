import usys as sys
sys.path.append('') # See: https://github.com/micropython/micropython/issues/6419

# Initialize 
import lvgl as lv
import time
from lv_utils import event_loop
from math import floor
import SDL

import uasyncio

from decotengu.engine import Engine
from decotengu.model import ZH_L16B_GF

model = ZH_L16B_GF(gf_low=0.30, gf_high=0.80)
engine = Engine(model=model)
engine.add_gas(depth=0, o2=21)

lv.init()
'''
## Demo Computer
Main
|__Plan
|
|__Setup
|  |__Time
|  |
|  |__Gasses
|  |
|  |__Model
|
|__History
|
|__Dive

## Shearwater
Main
|__Turn Off
|__Select Gas
|__Dive Setup
|__|__Define Gas
|__|__Dive Planner
|__|__Convserv
|__|__NDL
|__|__Brightness
|__Dive Log
|__|__Display Log
|__|__Upload Log
|__|__Next Log
|__|__Restore
|__|__Delete Logs
|__System Setup
|__|__Mode
|__|__Deco
|__|__Centre Row
|__|__Gasses
|__|__Display
|__|__Compass
|__|__System
|__|__Advanced
'''

def abs_p_to_depth(pressure):
    return (pressure - 1) * 10


class Gas():
    def __init__(self, o2, he, enabled=True):
        self.o2 = o2
        self.he = he
        self.enabled = enabled
        self.ppo2 = 1.4
        self._mod = 'auto'
    
    @property
    def name(self):
        if self.o2==0.21 and self.he==0:
            return 'Air'
        elif self.he==0:
            return f'EAN{int(self.o2*100)}'
        elif self._n2==0:
            return f'Heliox{int(self.o2*100)}'
        else:
            return f'Tri{int(self.o2*100)}/{int(self.he*100)}'

    @property
    def o2(self):
        try: return self._o2
        except: return 0

    @o2.setter
    def o2(self, val):
        self._o2 = val
        self._n2 = self.o2 - self.he

    @property
    def he(self):
        try: return self._he
        except: return 0

    @he.setter
    def he(self, val):
        self._he = val
        self._n2 = self.o2 - self.he

    @property
    def mod(self):
        if self._mod == 'auto':
            abs_p = self.ppo2 / self.o2
            return floor(abs_p_to_depth(abs_p))
        return self._mod

    @mod.setter
    def mod(self, val):
        assert val=='auto' or type(val) == int
        self._mod = val


async def update_time(labels, seconds=False):
    old_time = ''
    while 1:
        if old_time != f'{time.localtime()[3]:02}:{time.localtime()[4]:02}':
            old_time = f'{time.localtime()[3]:02}:{time.localtime()[4]:02}'
            for label in labels:
                label.set_text(old_time)
        await uasyncio.sleep_ms(25)


def physical_btn(parent, align, x, y, size=20):
    btn = lv.btn(parent)
    btn.align(align, x, y)
    btn.set_size(40,40)
    btn.set_style_radius(size//2, lv.PART.MAIN)
    return btn


class lv_obj_extended(lv.obj):
    def __init__(self, *args, **kwargs):
        super(lv_obj_extended, self).__init__(*args, **kwargs)
        self.visible = True
        self.scrollable = False

    @property
    def visible(self):
        return not self.has_flag(lv.obj.FLAG.HIDDEN)

    @visible.setter
    def visible(self, show):
        if show==True: self.clear_flag(lv.obj.FLAG.HIDDEN)
        elif show==False: self.add_flag(lv.obj.FLAG.HIDDEN)

    def toggle_visibility(self):
        self.visible = not self.visible

    @property
    def scrollable(self):
        return not self.has_flag(lv.obj.FLAG.SCROLLABLE)

    @scrollable.setter
    def scrollable(self, scroll):
        if scroll==True: self.add_flag(lv.obj.FLAG.SCROLLABLE)
        elif scroll==False: self.clear_flag(lv.obj.FLAG.SCROLLABLE)

    def toggle_scrollability(self):
        self.scrollable = not self.scrollable

    def set_style_pad(self, top=0, bottom=0, right=0, left=0):
        self.set_style_pad_top(top, lv.PART.MAIN)
        self.set_style_pad_bottom(bottom, lv.PART.MAIN)
        self.set_style_pad_right(right, lv.PART.MAIN)
        self.set_style_pad_left(left, lv.PART.MAIN)


class labled_btn(lv.btn):
    def __init__(self, parent, name, id=0):
        super(labled_btn, self).__init__(parent)
        self.name = name
        self.id = id
        self.label = lv.label(self)
        self.label.set_text(self.name)
        self.label.align_to(self, lv.ALIGN.CENTER, 0, 0)
        self.label.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN)

    def set_size(self, width, height):
        super(labled_btn, self).set_size(width, height)
        # self.label.set_size(width, height)
        self.label.align_to(self, lv.ALIGN.CENTER, 0, 0)

    def highlight(self, val):
        if val: self.set_style_bg_color(lv.palette_main(lv.PALETTE.RED), lv.PART.MAIN)
        else: self.set_style_bg_color(lv.palette_main(lv.PALETTE.BLUE), lv.PART.MAIN)

    def set_style_pad(self, top=0, bottom=0, right=0, left=0):
        self.set_style_pad_top(top, lv.PART.MAIN)
        self.set_style_pad_bottom(bottom, lv.PART.MAIN)
        self.set_style_pad_right(right, lv.PART.MAIN)
        self.set_style_pad_left(left, lv.PART.MAIN)


class Screen(lv_obj_extended):
    INFO = const(0)
    SETTING = const(1)
    HOME = const(2)

    def __init__(self, parent, type_, *args, **kwargs):
        super(Screen, self).__init__(parent, *args, **kwargs)
        self.parent = parent
        self.type = type_
        self.set_size(320, 240)
        self.set_style_pad_ver(0, lv.PART.MAIN)
        self.set_style_pad_hor(0, lv.PART.MAIN)
        self.set_style_radius(0, lv.PART.MAIN)
        self.set_style_border_width(1, lv.PART.MAIN)
        self.align_to(self.parent, lv.ALIGN.CENTER, 0, 0)
        self.set_style_bg_color(lv.color_make(10,10,10), lv.PART.MAIN)
        self.scrollable = False
        self.visible = False

    def show(self):
        self.visible = True


class HomeScreen(Screen):
    def __init__(self, controller, type_, gas_list, *args, **kwargs):
        super(HomeScreen, self).__init__(controller, type_)
        self.gas_list = gas_list
        col_dsc = [64, 64, 64, 64, 64, lv.GRID_TEMPLATE.LAST]
        row_dsc = [30, 35, 35, 35, 35, 35, 35, lv.GRID_TEMPLATE.LAST]
        self.set_style_grid_column_dsc_array(col_dsc, 0)
        self.set_style_grid_row_dsc_array(row_dsc, 0)
        self.set_style_pad_column(0, lv.PART.MAIN)
        self.set_style_pad_row(0, lv.PART.MAIN)
        self.center()
        self.set_layout(lv.LAYOUT_GRID.value)
        self.update_layout()
        self.x_size = 320//5
        self.y_size = 240//6

    def show(self):
        self.visible = True
        num_gasses = len(self.gas_list)
        
        l_menu = lv.label(self)
        l_menu.set_text(f'{lv.SYMBOL.LEFT} Menu')
        l_menu.set_style_text_font(lv.font_montserrat_14, lv.PART.MAIN)
        l_menu.set_style_text_color(lv.color_make(255, 0, 0), lv.PART.MAIN)
        l_menu.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 1, lv.GRID_ALIGN.STRETCH, 1, 1)

        self.l_time = lv.label(self)
        self.l_time.set_style_text_font(lv.font_montserrat_48, lv.PART.MAIN)
        self.l_time.set_style_text_color(lv.color_white(), lv.PART.MAIN)
        self.l_time.set_text('')
        self.l_time.set_grid_cell(lv.GRID_ALIGN.CENTER, 0, 3, lv.GRID_ALIGN.CENTER, 1, 2)

        for i in range(num_gasses):
            cont = lv_obj_extended(self)
            cont.set_style_border_width(0, lv.PART.MAIN)
            cont.set_style_radius(0, lv.PART.MAIN)
            colour = lv.color_make(200,200,200) if self.gas_list[i].enabled else lv.color_make(80,80,80)
            cont.set_style_bg_color(colour, lv.PART.MAIN)
            cont.set_grid_cell(lv.GRID_ALIGN.STRETCH, i, 1, lv.GRID_ALIGN.STRETCH, 6, 1)

            label = lv.label(cont)
            label.set_text(self.gas_list[i].name)
            label.align_to(cont,lv.ALIGN.CENTER, 0, 0)
            label.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN)


class GasScreen(Screen):
    def __init__(self, parent, type_, gas_list, *args, **kwargs):
        super(GasScreen, self).__init__(parent, type_, *args, **kwargs)
        self.gas_list = gas_list
        self.num_gasses = len(gas_list)
        self.gas_table = self.create_gas_table()

        self.edit_btns = []
        for i in range(self.num_gasses):
            btn = labled_btn(self.gas_table, 'Edit', id=i)
            btn.set_size(40, 20)
            btn.align_to(self.gas_table, lv.ALIGN.TOP_RIGHT, -10, 5 + 30*(i+1))
            btn.add_event_cb(self._cb_edit, lv.EVENT.CLICKED, None)
            self.edit_btns.append(btn)
        
        btn = labled_btn(self, 'Menu', id=self.num_gasses)
        btn.set_size(80, 20)
        btn.align_to(self.gas_table, lv.ALIGN.TOP_RIGHT, -20, 5 + 30*(self.num_gasses+1))
        btn.add_event_cb(self._cb_menu_btn, lv.EVENT.CLICKED, None)
        self.edit_btns.append(btn)

        self.focus_index = 0
        self.edit_btns[self.focus_index].highlight(True)

    def _cb_next(self, inc):
        def cb(event):
            self.edit_btns[self.focus_index % (self.num_gasses+1)].highlight(False)
            self.focus_index += inc
            self.edit_btns[self.focus_index % (self.num_gasses+1)].highlight(True)
        return cb

    def _cb_select(self, event):
        lv.event_send(self.edit_btns[self.focus_index], lv.EVENT.CLICKED, None)

    def _cb_edit(self, event):
        GasEdit(self, self.parent, self.focus_index)

    def _draw_part_event_cb(self):
        def cb(e):
            obj = e.get_target()
            dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
            # If the cells are drawn../
            if dsc.part == lv.PART.ITEMS:
                row = dsc.id //  obj.get_col_cnt()

                # Make the texts in the first cell center aligned
                if row == 0:
                    dsc.rect_dsc.bg_color = lv.palette_main(lv.PALETTE.BLUE).color_mix(dsc.rect_dsc.bg_color, lv.OPA._80)
                    dsc.rect_dsc.bg_opa = lv.OPA.COVER


                # Make every 2nd row grayish
                if not self.gas_list[row-1].enabled and row>0:
                    dsc.rect_dsc.bg_color = lv.palette_main(lv.PALETTE.GREY).color_mix(dsc.rect_dsc.bg_color, lv.OPA._100)
                    dsc.rect_dsc.bg_opa = lv.OPA.COVER

        return cb

    def create_gas_table(self):
        gas_table = lv.table(self)
        gas_table.set_size(310,180)
        gas_table.align_to(self, lv.ALIGN.CENTER, 0, 0)
        gas_table.clear_flag(lv.obj.FLAG.SCROLLABLE)

        gas_table.set_row_cnt(self.num_gasses+1)
        gas_table.add_event_cb(self._draw_part_event_cb(), lv.EVENT.DRAW_PART_BEGIN, None)
        gas_table.set_style_pad_ver(7, lv.PART.ITEMS)
        gas_table.set_style_pad_hor(8, lv.PART.ITEMS)
        gas_table.set_style_border_width(0, lv.PART.MAIN)
        gas_table.set_style_bg_color(lv.color_make(10,10,10), lv.PART.MAIN)

        gas_table.set_col_cnt(6)
        gas_table.set_cell_value(0, 0, '#')
        gas_table.set_cell_value(0, 1, 'Name')
        gas_table.set_cell_value(0, 2, 'O2')
        gas_table.set_cell_value(0, 3, 'He')
        gas_table.set_cell_value(0, 4, 'MOD')

        gas_table.set_col_width(0, 20)
        gas_table.set_col_width(1, 80)
        gas_table.set_col_width(2, 50)
        gas_table.set_col_width(3, 50)
        gas_table.set_col_width(4, 60)
        gas_table.set_col_width(5, 60)

        for row in range(self.num_gasses):
            self._draw_gas_table_row(gas_table, row)

        return gas_table

    def _draw_gas_table_row(self, gas_table, row):
        gas_table.set_cell_value(row+1, 0, str(row+1))
        gas_table.set_cell_value(row+1, 1, self.gas_list[row].name)
        gas_table.set_cell_value(row+1, 2, f"{int(self.gas_list[row].o2*100)}%")
        gas_table.set_cell_value(row+1, 3, f"{int(self.gas_list[row].he*100)}%")
        gas_table.set_cell_value(row+1, 4, f"{(self.gas_list[row].mod)}m")

    def show(self, focus=0):
        super(GasScreen, self).show()
        self.focus_index = focus
        self.edit_btns[self.focus_index].highlight(True)
        self._draw_gas_table_row(self.gas_table, self.focus_index)
        self.parent.btn_tl.remove_event_dsc(self.parent.cb_tl)
        self.parent.btn_bl.remove_event_dsc(self.parent.cb_bl)
        self.parent.btn_br.remove_event_dsc(self.parent.cb_br)

        self.cb_tl = self.parent.btn_tl.add_event_cb(self._cb_select, lv.EVENT.CLICKED, None)
        self.cb_bl = self.parent.btn_bl.add_event_cb(self._cb_next(-1), lv.EVENT.CLICKED, None)
        self.cb_br = self.parent.btn_br.add_event_cb(self._cb_next(1), lv.EVENT.CLICKED, None)

    def _cb_menu_btn(self, event):
        self.edit_btns[-1].highlight(False)
        self.parent.menu_bar.toggle_visibility()
        self.parent.btn_tl.remove_event_dsc(self.cb_tl)
        self.parent.btn_bl.remove_event_dsc(self.cb_bl)
        self.parent.btn_br.remove_event_dsc(self.cb_br)
        self.parent.cb_tl = self.parent.btn_tl.add_event_cb(self.parent._cb_centre_btn, lv.EVENT.CLICKED, None)
        self.parent.define_nav_cb()


class GasEdit(lv_obj_extended):
    def __init__(self, parent, controller, index):
        super(GasEdit, self).__init__(parent)
        self.parent = parent
        self.controller = controller
        self.index = index
        self.edit_btns = []
        self.set_size(200, 170)
        self.align_to(self.parent, lv.ALIGN.CENTER, 0, 0)

        btn = labled_btn(self, f'Oxygen: {int(self.parent.gas_list[self.index].o2*100)}%')
        btn.add_event_cb(self._cb_o2_btn, lv.EVENT.CLICKED, None)
        btn.set_size(140, 20)
        btn.align_to(self, lv.ALIGN.TOP_MID, 0, 0)
        self.edit_btns.append(btn)

        btn = labled_btn(self, f'Helium: {int(self.parent.gas_list[self.index].he*100)}%')
        btn.add_event_cb(self._cb_he_btn, lv.EVENT.CLICKED, None)
        btn.set_size(140, 20)
        btn.align_to(self, lv.ALIGN.TOP_MID, 0, 30)
        self.edit_btns.append(btn)


        btn = labled_btn(self, f'Enabled: {self._text_enabled(self.parent.gas_list[self.index].enabled)}')
        btn.add_event_cb(self._cb_enable_btn, lv.EVENT.CLICKED, None)
        btn.set_size(140, 20)
        btn.align_to(self, lv.ALIGN.TOP_MID, 0, 60)
        self.edit_btns.append(btn)

        btn = labled_btn(self, f'PPO2: {self.parent.gas_list[self.index].ppo2}')
        btn.add_event_cb(self._cb_ppo2_btn, lv.EVENT.CLICKED, None)
        btn.set_size(140, 20)
        btn.align_to(self, lv.ALIGN.TOP_MID, 0, 90)
        self.edit_btns.append(btn)

        btn = labled_btn(self, 'Done')
        btn.add_event_cb(self._cb_done_btn, lv.EVENT.CLICKED, None)
        btn.set_size(140, 20)
        btn.align_to(self, lv.ALIGN.TOP_MID, 0, 120)
        self.edit_btns.append(btn)

        self.show()

    def _text_enabled(self, enabled):
        text = 'Yes' if enabled else 'No'
        return text

    def _cb_next(self, inc):
        def cb(event):
            self.edit_btns[self.focus_index % (5)].highlight(False)
            self.focus_index = (inc + self.focus_index) % 5
            self.edit_btns[self.focus_index % (5)].highlight(True)
        return cb

    def _cb_edit(self, event):
        lv.event_send(self.edit_btns[self.focus_index], lv.EVENT.CLICKED, None)

    def _cb_o2_btn(self, event):
        self.parent.gas_list[self.index].o2 += 0.01
        if self.parent.gas_list[self.index].o2 > 1 - self.parent.gas_list[self.index].he:
            self.parent.gas_list[self.index].o2 = 0.01
        self.edit_btns[self.focus_index].label.set_text(f'Oxygen: {int(self.parent.gas_list[self.index].o2*100)}%')

    def _cb_he_btn(self, event):
        self.parent.gas_list[self.index].he += 0.01
        if self.parent.gas_list[self.index].he > 1 - self.parent.gas_list[self.index].o2:
            self.parent.gas_list[self.index].he = 0.01
        self.edit_btns[self.focus_index].label.set_text(f'Helium: {int(self.parent.gas_list[self.index].he*100)}%')

    def _cb_enable_btn(self, event):
        self.parent.gas_list[self.index].enabled = not self.parent.gas_list[self.index].enabled
        self.edit_btns[self.focus_index].label.set_text(f'Enabled: {self._text_enabled(self.parent.gas_list[self.index].enabled)}')

    def _cb_ppo2_btn(self, event):
        self.parent.gas_list[self.index].ppo2 = 1.6 if self.parent.gas_list[self.index].ppo2==1.4 else 1.4
        self.edit_btns[self.focus_index].label.set_text(f'PPO2: {(self.parent.gas_list[self.index].ppo2)}')

    def _cb_done_btn(self, event):
        self.controller.btn_tl.remove_event_dsc(self.cb_tl)
        self.controller.btn_bl.remove_event_dsc(self.cb_bl)
        self.controller.btn_br.remove_event_dsc(self.cb_br)
        self.delete()
        self.parent.show(focus=self.index)

    def show(self, focus=0):
        self.focus_index = focus
        self.edit_btns[self.focus_index].highlight(True)
        self.controller.btn_tl.remove_event_dsc(self.parent.cb_tl)
        self.controller.btn_bl.remove_event_dsc(self.parent.cb_bl)
        self.controller.btn_br.remove_event_dsc(self.parent.cb_br)

        self.cb_tl = self.controller.btn_tl.add_event_cb(self._cb_edit, lv.EVENT.CLICKED, None)
        self.cb_bl = self.controller.btn_bl.add_event_cb(self._cb_next(-1), lv.EVENT.CLICKED, None)
        self.cb_br = self.controller.btn_br.add_event_cb(self._cb_next(1), lv.EVENT.CLICKED, None)


class PlanningScreen(Screen):
    def __init__(self, parent, type_, gas_list, *args, **kwargs):
        super(PlanningScreen, self).__init__(parent, type_, *args, **kwargs)
        col_dsc = [58, 58, 58, 58, 88, lv.GRID_TEMPLATE.LAST]
        row_dsc = [30, 35, 35, 35, 35, 35, 35, lv.GRID_TEMPLATE.LAST]
        self.set_style_grid_column_dsc_array(col_dsc, 0)
        self.set_style_grid_row_dsc_array(row_dsc, 0)
        self.set_style_pad_column(0, lv.PART.MAIN)
        self.set_style_pad_row(0, lv.PART.MAIN)
        self.center()
        self.set_layout(lv.LAYOUT_GRID.value)
        self.update_layout()
        self.x_size = 320//5
        self.y_size = 240//6

        self.letter_colour = ['#ffffff ', '#00ffff ']
        self.letter_index = [8, 18, 28]
        
        self.depth_val = [0,3,0]
        self.time_val = [0,2,5]
        self.plan_setting = [self.depth_val, self.time_val]

        self.btn_plan = labled_btn(self, 'Plan')
        self.btn_plan.set_grid_cell(lv.GRID_ALIGN.CENTER, 4, 1, lv.GRID_ALIGN.CENTER, 1, 1)
        self.btn_plan.set_size(70, 29)

        self.c_bottom = lv_obj_extended(self)
        self.c_bottom.set_grid_cell(lv.GRID_ALIGN.CENTER, 0, 5, lv.GRID_ALIGN.CENTER, 6, 1)
        self.c_bottom.set_width(lv.pct(100))
        self.c_bottom.set_height(40)
        self.c_bottom.set_style_pad(2,2,2,2)

        btn_right = labled_btn(self.c_bottom, lv.SYMBOL.RIGHT)
        btn_right.set_size(50, 29)
        btn_right.align_to(btn_right.get_parent(), lv.ALIGN.TOP_RIGHT, 0, 0)

        btn_left = labled_btn(self.c_bottom, lv.SYMBOL.LEFT)
        btn_left.align(lv.ALIGN.TOP_LEFT, 0, 0)
        btn_left.set_size(50, 29)

        self.btn_menu = labled_btn(self.c_bottom, 'Menu')
        self.btn_menu.align(lv.ALIGN.TOP_MID, 0, 0)
        self.btn_menu.set_size(100, 29)

        self.chart = lv.chart(self)
        self.chart.set_grid_cell(lv.GRID_ALIGN.CENTER, 0, 5, lv.GRID_ALIGN.CENTER, 2, 4)
        self.chart.set_size(300, 120)

        self.setting_grid()


    def setting_grid(self):
        self.l_settings = []
        self.focus_index = 0

        l_depth = lv.label(self)
        l_depth.set_text('Depth: ')
        l_depth.set_style_text_font(lv.font_montserrat_14, lv.PART.MAIN)
        l_depth.set_style_text_color(lv.color_make(255, 0, 0), lv.PART.MAIN)
        l_depth.set_grid_cell(lv.GRID_ALIGN.END, 0, 1, lv.GRID_ALIGN.CENTER, 1, 1)

        self.l_depth_set = lv.label(self)
        self.l_depth_set.set_style_text_font(lv.font_montserrat_14, lv.PART.MAIN)
        self.l_depth_set.set_recolor(True)
        self.l_depth_set.set_grid_cell(lv.GRID_ALIGN.START, 1, 1, lv.GRID_ALIGN.CENTER, 1, 1)
        self.l_settings.append(self.l_depth_set)

        l_time = lv.label(self)
        l_time.set_text('Time: ')
        l_time.set_style_text_font(lv.font_montserrat_14, lv.PART.MAIN)
        l_time.set_style_text_color(lv.color_make(255, 0, 0), lv.PART.MAIN)
        l_time.set_grid_cell(lv.GRID_ALIGN.END, 2, 1, lv.GRID_ALIGN.CENTER, 1, 1)

        self.l_time_set = lv.label(self)
        self.l_time_set.set_style_text_font(lv.font_montserrat_14, lv.PART.MAIN)
        self.l_time_set.set_recolor(True)
        self.l_time_set.set_grid_cell(lv.GRID_ALIGN.START, 3, 1, lv.GRID_ALIGN.CENTER, 1, 1)
        self.l_settings.append(self.l_time_set)

        self.setting_text()

    def inc_val(self):
        reduced_focus = self.focus_index % 8
        if reduced_focus < 6:
            setting_index = reduced_focus // 3
            val_index = reduced_focus % 3

            current = self.plan_setting[setting_index][val_index]

            new_val = current + 1 if current < 9 else 0
            self.plan_setting[setting_index][val_index] = new_val

        self.setting_text()

    def setting_text(self):
        reduced_focus = self.focus_index % 8

        if reduced_focus == 6:
            self.btn_plan.highlight(True)
        else:
            self.btn_plan.highlight(False)

        if reduced_focus == 7:
            self.btn_menu.highlight(True)
        else:
            self.btn_menu.highlight(False)

        self.l_depth_set.set_text(f'{self.letter_colour[reduced_focus==0]}{self.plan_setting[0][0]}#' +
                                  f'{self.letter_colour[reduced_focus==1]}{self.plan_setting[0][1]}#' +
                                  f'{self.letter_colour[reduced_focus==2]}{self.plan_setting[0][2]}#')

        self.l_time_set.set_text(f'{self.letter_colour[reduced_focus==3]}{self.plan_setting[1][0]}#' +
                                 f'{self.letter_colour[reduced_focus==4]}{self.plan_setting[1][1]}#' +
                                 f'{self.letter_colour[reduced_focus==5]}{self.plan_setting[1][2]}#')

    def limits(self, points):
        x = 0
        y = 0
        for p in points:
            x = p[0] if p[0] > x else x
            y = p[1] if p[1] > y else y
        return (x, y)

    def draw_chart(self, chart, points):
        limit = self.limits(points)
        chart_points = []
        for p in points:
            x = round(p[0] * 260/limit[0])
            y = round(p[1] * 85/limit[1])
            chart_points.append({'x': x, 'y': y})


        chart_line = lv.line(chart)
        chart_line.set_points(chart_points, len(chart_points))
        chart_line.set_style_line_width(2, lv.PART.MAIN)
        chart_line.set_style_line_color(lv.color_hex(0xff0000), lv.PART.MAIN)
        chart_line.align(lv.ALIGN.TOP_LEFT, 5, 5)
        return chart_line

    def plan(self):
        depth = self.plan_setting[0][0] * 100 + self.plan_setting[0][1] * 10 + self.plan_setting[0][2]
        time = self.plan_setting[1][0] * 100 + self.plan_setting[1][1] * 10 + self.plan_setting[1][2]
        profile = [step for step in engine.calculate(depth, time)]
        points = [[item.time, round((item.abs_p-1)*10)] for item in profile]
        line = self.draw_chart(self.chart, points)

    def _cb_next(self, event):
        self.focus_index += 1
        self.setting_text()

    def _cb_prev(self, event):
        self.focus_index -= 1
        self.setting_text()

    def _cb_inc(self, event):
        reduced_focus = self.focus_index % 8

        if reduced_focus == 7:
            self._cb_menu_btn(event)
        elif reduced_focus == 6:
            self.plan()
        else:
            self.inc_val()

    def show(self, focus=0):
        super(PlanningScreen, self).show()
        self.focus_index = focus
        self.parent.btn_tl.remove_event_dsc(self.parent.cb_tl)
        self.parent.btn_bl.remove_event_dsc(self.parent.cb_bl)
        self.parent.btn_br.remove_event_dsc(self.parent.cb_br)

        self.cb_tl = self.parent.btn_tl.add_event_cb(self._cb_inc, lv.EVENT.CLICKED, None)
        self.cb_bl = self.parent.btn_bl.add_event_cb(self._cb_prev, lv.EVENT.CLICKED, None)
        self.cb_br = self.parent.btn_br.add_event_cb(self._cb_next, lv.EVENT.CLICKED, None)

    def _cb_menu_btn(self, event):
        self.c_bottom.toggle_visibility()
        self.parent.menu_bar.toggle_visibility()
        self.parent.btn_tl.remove_event_dsc(self.cb_tl)
        self.parent.btn_bl.remove_event_dsc(self.cb_bl)
        self.parent.btn_br.remove_event_dsc(self.cb_br)
        self.parent.cb_tl = self.parent.btn_tl.add_event_cb(self.parent._cb_centre_btn, lv.EVENT.CLICKED, None)
        self.parent.define_nav_cb()


class Controller(lv.obj):
    def __init__(self, *args, **kwargs):
        super(Controller, self).__init__(*args, **kwargs)
        self.top_bar = TopBar(self, self)
        self.menu_bar = MenuBar(self, 'Main', self)

        self.btn_tl = physical_btn(self, lv.ALIGN.TOP_LEFT, 20, 20, 40)
        self.btn_tr = physical_btn(self, lv.ALIGN.TOP_RIGHT, -20, 20, 40)
        self.btn_bl = physical_btn(self, lv.ALIGN.BOTTOM_LEFT, 20, -20, 40)
        self.btn_br = physical_btn(self, lv.ALIGN.BOTTOM_RIGHT, -20, -20, 40)

        self.cb_tl = self.btn_tl.add_event_cb(self._cb_centre_btn, lv.EVENT.CLICKED, None)
        self.cb_tr = self.btn_tr.add_event_cb(self._cb_home_btn, lv.EVENT.CLICKED, None)
        self.define_nav_cb()

    def define_nav_cb(self):
        self.cb_bl = self.btn_bl.add_event_cb(self.menu_bar.prev_button, lv.EVENT.CLICKED, None)
        self.cb_br = self.btn_br.add_event_cb(self.menu_bar.next_button, lv.EVENT.CLICKED, None)

    def _cb_home_btn(self, event):
        if self.active_screen != self.screens['Main']:
            self.menu_bar.previous_menu = 'Close'
            self.menu_bar.create_buttons(self.menu_bar.menu_list['Main'])
            self.active_screen = 'Main'
        self.menu_bar.visible=False

    def _cb_centre_btn(self, event):
        if self.menu_bar.visible:
            self.menu_bar.cntr_button(event)
        else:
            self.menu_bar.toggle_visibility()

    def _cb_toggle_btn(self, event):
        self.menu_bar.toggle_visibility()

    def set_screens(self, screens):
        self.screens = screens
        self.active_screen = 'Main'

    @property
    def active_screen(self):
        return self._active_screen

    @active_screen.setter
    def active_screen(self, name):
        try: self._active_screen.visible = False
        except: pass
        self._active_screen = self.screens[name]
        self.screens[name].show()

        self.top_bar.set_parent(self._active_screen)
        self.top_bar.set_alignment()
        if name=='Main':
            self.top_bar.title = 'PyDive Leviathan'
        else:
            self.top_bar.title = name

        self.menu_bar.set_parent(self._active_screen)
        self.menu_bar.set_alignment()


class MenuBar(lv_obj_extended):

    menu_list = {
        'Main':  ['Plan', 'Setup', 'History', 'Dive', 'Close'],
        'Plan': ['Main'],
        'Setup': ['Time', 'Gasses', 'Model', 'Main'],
        'Time': ['Setup'],
        'Gasses': ['Setup'],
        'Model': ['Setup'],
        'History': ['Main'],
        'Dive': ['Main'],
        }

    def __init__(self, controller, initial_menu='Main', *args, **kwargs):
        super(MenuBar, self).__init__(*args, **kwargs)
        self.index = 0
        self.initial_menu = initial_menu
        self.previous_menu = ''
        self.controller = controller

        self.visible = False
        self.clear_flag(lv.obj.FLAG.SCROLLABLE)
        self.parent = self.get_parent()
        self.set_size(self.parent.get_width(), 40)
        self.set_alignment()
        self.direction_buttons()
        self.create_buttons(self.menu_list[initial_menu])

    def set_alignment(self):
        self.align_to(self.get_parent(), lv.ALIGN.BOTTOM_MID, 0, 12)

    def direction_buttons(self):
        def _cb_left_btn(event):
            self.prev_button()
        def _cb_right_btn(event):
            self.next_button()

        left_button = lv.btn(self)
        left_button.set_size(20, 20)
        left_button.add_event_cb(_cb_left_btn, lv.EVENT.CLICKED, None)
        left_button.align(lv.ALIGN.CENTER, -140, -5)
        left_label = lv.label(left_button)
        left_label.set_text(lv.SYMBOL.LEFT)
        left_label.align(lv.ALIGN.CENTER,0,0)

        right_button = lv.btn(self)
        right_button.set_size(20, 20)
        right_button.add_event_cb(_cb_right_btn, lv.EVENT.CLICKED, None)
        right_button.align(lv.ALIGN.CENTER, 140, -5)
        right_label = lv.label(right_button)
        right_label.set_text(lv.SYMBOL.RIGHT)
        right_label.align(lv.ALIGN.CENTER,0,0)

    def create_buttons(self, button_list):
        self.clean()
        self.direction_buttons()
        self.previous_menu = button_list[-1]
        self.buttons  = [self._create_btn(button) for button in button_list[:-1]]
        self.buttons.append(self._create_btn('Back'))
        self.show_buttons()

    def _cb_btn_name(self, name):
        name = self.previous_menu if name == 'Back' else name  

        if name == 'Close':
            def _cb_close(event):
                self.toggle_visibility()
            return _cb_close

        def _cb_btn(event):
            self.index = 0
            self.create_buttons(self.menu_list[name])
        return _cb_btn

    def _create_btn(self, button):
        new_button = labled_btn(self, button)
        new_button.set_size(80, 20)
        new_button.add_flag(lv.obj.FLAG.HIDDEN)
        new_button.add_event_cb(self._cb_btn_name(button), lv.EVENT.CLICKED, None)
        return [new_button, button]

    def _del(self):
        self.delete()

    def show_buttons(self, index=None):
        index = index if index else self.index
        num_buttons = len(self.buttons)
        for i in range(-1, 2):
            if i == -1 and num_buttons < 3: continue
            if i == 1 and num_buttons == 1: break
            btn = self.buttons[(i+index)%num_buttons][0]
            btn.clear_flag(lv.obj.FLAG.HIDDEN)
            btn.align(lv.ALIGN.TOP_MID, i*85, -13)
            if i == 0: 
                btn.highlight(True)
                self.centre_name = self.buttons[(i+index)%num_buttons][1]
            else: btn.highlight(False)

        for i in range(2, num_buttons-1):
            btn = self.buttons[(i+index)%num_buttons][0]
            btn.add_flag(lv.obj.FLAG.HIDDEN)

    def cntr_button(self, *args):
        name = self.previous_menu if self.centre_name == 'Back' else self.centre_name

        if name == 'Close':
            self.create_buttons(self.menu_list[self.initial_menu])
            self.controller.active_screen = self.initial_menu
            self.toggle_visibility()
            return

        self.index = 0
        self.create_buttons(self.menu_list[name])

        if len(self.menu_list[name]) == 1:
            self.controller.active_screen = name
            self.visible = False
            return

    def prev_button(self, *args):
        self.index-=1
        self.show_buttons()

    def next_button(self, *args):
        self.index+=1
        self.show_buttons()


class TopBar(lv_obj_extended):
    def __init__(self, controller, *args, **kwargs):
        super(TopBar, self).__init__(*args, **kwargs)
        self.set_size(320, 40)
        self.set_alignment()
        self.set_style_bg_color(lv.color_make(30,30,30), lv.PART.MAIN)
        self.clear_flag(lv.obj.FLAG.SCROLLABLE)

        self.l_title = lv.label(self)
        self.l_title.set_text('ESPy Dive')
        self.l_title.align(lv.ALIGN.BOTTOM_MID, 0, 10)
        self.l_title.set_style_text_color(lv.color_make(255, 255, 255), lv.PART.MAIN)

        self.l_time = lv.label(self)
        self.l_time.set_text('TIME')
        self.l_time.align(lv.ALIGN.BOTTOM_LEFT, 0, 10)
        self.l_time.set_style_text_color(lv.color_make(255, 255, 255), lv.PART.MAIN)

        self.l_bat = lv.label(self)
        self.l_bat.set_text(f'{lv.SYMBOL.CHARGE}{lv.SYMBOL.BATTERY_3}')
        self.l_bat.align(lv.ALIGN.BOTTOM_RIGHT, 0, 10)
        self.l_bat.set_style_text_color(lv.color_make(200, 200, 0), lv.PART.MAIN)

        self.l_blue = lv.label(self)
        self.l_blue.set_text(lv.SYMBOL.BLUETOOTH)
        self.l_blue.align(lv.ALIGN.BOTTOM_RIGHT, -50, 10)
        self.l_blue.set_style_text_color(lv.color_make(70, 70, 70), lv.PART.MAIN)

    def set_alignment(self):
        self.align_to(self.get_parent(), lv.ALIGN.TOP_MID, 0, -12)

    @property
    def title(self):
        return self.l_title.get_text()

    @title.setter
    def title(self, text):
        self.l_title.set_text(text)


class driver:
    def init_gui_SDL(self):


        SDL.init(auto_refresh=False)
        self.event_loop = event_loop(refresh_cb = SDL.refresh)

        # Register SDL display driver.

        disp_buf1 = lv.disp_draw_buf_t()
        buf1_1 = bytes(320 * 10)
        disp_buf1.init(buf1_1, None, len(buf1_1)//4)
        disp_drv = lv.disp_drv_t()
        disp_drv.init()
        disp_drv.draw_buf = disp_buf1
        disp_drv.flush_cb = SDL.monitor_flush
        disp_drv.hor_res = 480
        disp_drv.ver_res = 240
        disp_drv.register()

        # Regsiter SDL mouse driver

        indev_drv = lv.indev_drv_t()
        indev_drv.init() 
        indev_drv.type = lv.INDEV_TYPE.POINTER
        indev_drv.read_cb = SDL.mouse_read
        indev_drv.register()
        
    def init_gui_esp32(self):

        # Initialize ILI9341 display

        from ili9XXX import ili9341

        self.disp = ili9341(dc=32, cs=33, power=-1, backlight=-1)

        # Register raw resistive touch driver

        """
        import rtch
        self.touch = rtch.touch(xp = 32, yp = 33, xm = 25, ym = 26, touch_rail = 27, touch_sense = 33)
        self.touch.init()
        indev_drv = lv.indev_drv_t()
        lv.indev_drv_init(indev_drv) 
        indev_drv.type = lv.INDEV_TYPE.POINTER
        indev_drv.read_cb = self.touch.read
        lv.indev_drv_register(indev_drv)
        """

        # Register xpt2046 touch driver

        from xpt2046 import xpt2046

        self.touch = xpt2046()

    def init_gui_stm32(self):
        import rk043fn48h as lcd

        hres = 480
        vres = 272

        # Register display driver
        self.event_loop = event_loop()
        lcd.init(w=hres, h=vres)
        disp_buf1 = lv.disp_draw_buf_t()
        buf1_1 = bytearray(hres * 50 * lv.color_t.__SIZE__)
        buf1_2 = bytearray(hres * 50 * lv.color_t.__SIZE__)
        disp_buf1.init(buf1_1, buf1_2, len(buf1_1) // lv.color_t.__SIZE__)
        disp_drv = lv.disp_drv_t()
        disp_drv.flush_cb = lcd.flush
        disp_drv.hor_res = hres
        disp_drv.ver_res = vres
        disp_drv.register()

        # Register touch sensor
        indev_drv = lv.indev_drv_t()
        indev_drv.init()
        indev_drv.type = lv.INDEV_TYPE.POINTER
        indev_drv.read_cb = lcd.ts_read
        indev_drv.register()

    def init_gui(self):

        # Identify platform and initialize it
        if not event_loop.is_running():
            try:
                self.init_gui_esp32()
                print('Initialised for esp32')
            except ImportError:
                pass

            try:
                self.init_gui_SDL()
                print('Initialised for SDL')
            except ImportError:
                pass

            try:
                self.init_gui_stm32()
                print('Initialised for stm32')
            except ImportError:
                pass


drv = driver()
drv.init_gui()

base = Controller()
base.set_style_bg_color(lv.color_make(1,1,1), lv.PART.MAIN)
base.clear_flag(base.FLAG.SCROLLABLE)

air = Gas(0.21, 0)
ean32 = Gas(0.32, 0, enabled=False)
ean36 = Gas(0.21, 0.35, enabled=False)
ean50 = Gas(0.50, 0)
ean80 = Gas(0.80, 0, enabled=False)

gas_list = [air, ean32, ean36, ean50, ean80]

scr_home = HomeScreen(base, Screen.INFO, gas_list)
scr_plan = PlanningScreen(base, Screen.SETTING, gas_list)
scr_set_time = Screen(base, Screen.SETTING)
scr_set_gas = GasScreen(base, Screen.SETTING, gas_list)
scr_set_model = Screen(base, Screen.SETTING)
scr_history = Screen(base, Screen.INFO)
scr_dive = Screen(base, Screen.INFO)

screens = {
    'Main': scr_home,
    'Plan': scr_plan,
    'Time': scr_set_time,
    'Gasses': scr_set_gas,
    'Model': scr_set_model,
    'History': scr_history,
    'Dive': scr_dive,
}

base.set_screens(screens)

lv.scr_load(base)

uasyncio.run(update_time([base.top_bar.l_time, scr_home.l_time]))
