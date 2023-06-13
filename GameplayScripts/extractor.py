from LViewPlus64 import *

import requests
import traceback
from pymongo import MongoClient

from config import DB_ADDRESS, DB_PORT, DB_USER, DB_PASS

from urllib3.exceptions import InsecureRequestWarning

requests.packages.urllib3.disable_warnings(category=InsecureRequestWarning)

client = MongoClient('mongodb://' + DB_USER + ':' +
                     DB_PASS + '@' + DB_ADDRESS + ':' + DB_PORT)

db = client['replaydata']

LViewPlus64_script_info = {
    "script": "Extractor",
    "author": "MiscellaneousStuff",
    "description": "Extracts observations for machine learning."
}

c = ['aatrox', 'ahri', 'akali', 'akshan', 'alistar', 'amumu', 'anivia', 'annie', 'aphelios', 'ashe', 'aurelionsol', 'azir', 'bard', 'belveth', 'blitzcrank', 'brand', 'braum', 'caitlyn', 'camille', 'cassiopeia', 'chogath', 'corki', 'darius', 'diana', 'drmundo', 'draven', 'ekko', 'elise', 'evelynn', 'ezreal', 'fiddlesticks', 'fiora', 'fizz', 'galio', 'gangplank', 'garen', 'gnar', 'gragas', 'graves', 'hecarim', 'heimerdinger', 'illaoi', 'irelia', 'ivern', 'janna', 'jarvaniv', 'jax', 'jayce', 'jhin', 'jinx', 'ksante', 'kaisa', 'kalista', 'karma', 'karthus', 'kassadin', 'katarina', 'kayle', 'kayn', 'kennen', 'khazix', 'kindred', 'kled', 'kogmaw', 'leblanc', 'leesin', 'leona', 'lillia', 'lissandra', 'lucian', 'lulu', 'lux', 'malphite', 'malzahar', 'maokai', 'masteryi', 'milio', 'missfortune', 'mordekaiser',
     'morgana', 'nami', 'nasus', 'nautilus', 'neeko', 'nidalee', 'nilah', 'nocturne', 'nunu', 'olaf', 'orianna', 'ornn', 'pantheon', 'poppy', 'pyke', 'qiyana', 'quinn', 'rakan', 'rammus', 'reksai', 'rell', 'renataglasc', 'renekton', 'rengar', 'riven', 'rumble', 'ryze', 'samira', 'sejuani', 'senna', 'seraphine', 'sett', 'shaco', 'shen', 'shyvana', 'singed', 'sion', 'sivir', 'skarner', 'sona', 'soraka', 'swain', 'sylas', 'syndra', 'tahmkench', 'taliyah', 'talon', 'taric', 'teemo', 'thresh', 'tristana', 'trundle', 'tryndamere', 'twistedfate', 'twitch', 'udyr', 'urgot', 'varus', 'vayne', 'veigar', 'velkoz', 'vi', 'viego', 'viktor', 'vladimir', 'volibear', 'warwick', 'monkeyking', 'xayah', 'xerath', 'xinzhao', 'yasuo', 'yone', 'yorick', 'yuumi', 'zac', 'zed', 'zeri', 'ziggs', 'zilean', 'zoe', 'zyra']


def LViewPlus64_load_cfg(cfg):
    pass


def LViewPlus64_save_cfg(cfg):
    pass


def LViewPlus64_draw_settings(game, ui):
    pass


snapshot_index = 0
gametime = 0
page_index = 0


def minionadv(red, blue):
    if red > blue:
        return "red"
    elif blue > red:
        return "blue"
    else:
        return "N/A"


def wave_pushed(bluebasered, bluel3red, bluel2red, redl2blue, redl3blue, redbaseblue):
    if sum([bluebasered, bluel3red, bluel2red]) > 0 and sum([redl3blue, redl2blue, redbaseblue]) == 0:
        return "red"
    elif sum([bluebasered, bluel3red, bluel2red]) == 0 and sum([redl3blue, redl2blue, redbaseblue]) > 0:
        return "blue"
    else:
        return "N/A"


def minions_in_area(x1, x2, y1, y2, positions):
    return len([tuple for tuple in positions if tuple[0] <= x2 and tuple[0] >= x1 and tuple[1] <= y2 and tuple[1] >= y1])


team = {
    100: "blue",
    200: "red"
}

minion_names = {
    "sru_chaosminionranged": "r_ranged",
    "sru_chaosminionsiege": "r_siege",
    "sru_chaosminionmelee": "r_melee",
    "sru_chaosminionsuper": "r_super",
    "sru_orderminionranged": "b_ranged",
    "sru_orderminionsiege": "b_siege",
    "sru_orderminionmelee": "b_melee",
    "sru_orderminionsuper": "b_super",
}


def dump(obj):
    # print("dump")
    global snapshot_index
    global gametime
    global page_index

    if (gametime == obj.time):
        return
    gametime = obj.time

    champlist = obj.champs

    others = []
    for item in obj.others:
        if item.name in c:
            champlist.append(item)
        else:
            others.append({
                "name": item.name,
                "x": round(item.pos.x, 4),
                "z": round(item.pos.z, 4),
                "hp": round(item.health, 4),
                "max_hp": item.max_health
            })
    jungle = []
    dragon = False
    herald = False
    baron = False
    for item in obj.jungle:
        jungle.append({
            "name": item.name,
            "x": round(item.pos.x, 4),
            "z": round(item.pos.z, 4),
            "hp": round(item.health, 4),
            "max_hp": item.max_health
        })
        if (item.name == 'dragon'):
            dragon = True
        if (item.name == 'herald'):
            herald = True
        if (item.name == 'baron'):
            baron = True

    minions = [{
        "name": minion_names[item.name],
        "x": round(item.pos.x, 4),
        "z": round(item.pos.z, 4),
        "hp": round(item.health, 4),
        "max_hp": item.max_health
    } for item in obj.minions]

    # redminionpos = [(minion.pos.x, minion.pos.z)
    #                 for minion in obj.minions if "chaos" in minion.name]
    # blueminionpos = [(minion.pos.x, minion.pos.z)
    #                  for minion in obj.minions if "order" in minion.name]
    # red_top_inner = len([tuple for tuple in blueminionpos if tuple[0] <=
    #                     4550 and tuple[0] >= 1400 and tuple[1] <= 13850 and tuple[1] >= 10350])
    # red_bot_inner = len([tuple for tuple in redminionpos if tuple[0] <=
    #                     14000 and tuple[0] >= 10400 and tuple[1] <= 4505 and tuple[1] >= 1200])
    # red_mid_inner = len([tuple for tuple in redminionpos if tuple[0] <=
    #                     8955 and tuple[0] >= 5800 and tuple[1] <= 8510 and tuple[1] >= 6200])
    # blue_top_inner = len([tuple for tuple in blueminionpos if tuple[0] <=
    #                      4550 and tuple[0] >= 1400 and tuple[1] <= 13850 and tuple[1] >= 10350])
    # blue_mid_inner = len([tuple for tuple in blueminionpos if tuple[0] <=
    #                      8955 and tuple[0] >= 5800 and tuple[1] <= 8510 and tuple[1] >= 6200])
    # blue_bot_inner = len([tuple for tuple in redminionpos if tuple[0] <=
    #                      14000 and tuple[0] >= 10400 and tuple[1] <= 4505 and tuple[1] >= 1200])

    # top_wave_pushed = wave_pushed(minions_in_area(0, 3651, 0, 3696, redminionpos),
    #                               minions_in_area(
    #                                   900, 2500, 4252, 6699, redminionpos),
    #                               minions_in_area(
    #                                   900, 2500, 6699, 10350, redminionpos),
    #                               minions_in_area(
    #                                   4550, 7952, 12500, 14000, blueminionpos),
    #                               minions_in_area(
    #                                   7952, 10481, 12500, 14000, blueminionpos),
    #                               minions_in_area(11134, 11207, 18000, 18000, blueminionpos))
    # if top_wave_pushed == 'N/A':
    #     top_wave_pushed = minionadv(red_top_inner, blue_top_inner)

    # mid_wave_pushed = wave_pushed(minions_in_area(0, 3651, 0, 3696, redminionpos),
    #                               minions_in_area(
    #                                   3651, 5048, 3696, 4812, redminionpos),
    #                               minions_in_area(
    #                                   5048, 5800, 4812, 6200, redminionpos),
    #                               minions_in_area(
    #                                   8955, 9767, 8510, 10113, blueminionpos),
    #                               minions_in_area(
    #                                   9767, 11134, 10113, 11207, blueminionpos),
    #                               minions_in_area(11134, 11207, 18000, 18000, blueminionpos))

    # if mid_wave_pushed == 'N/A':
    #     mid_wave_pushed = minionadv(red_mid_inner, blue_mid_inner)

    # bot_wave_pushed = wave_pushed(minions_in_area(0, 3651, 0, 3696, redminionpos),
    #                               minions_in_area(
    #                                   4281, 6919, 900, 2500, redminionpos),
    #                               minions_in_area(
    #                                   6919, 10400, 900, 2500, redminionpos),
    #                               minions_in_area(
    #                                   12000, 14500, 4505, 8226, blueminionpos),
    #                               minions_in_area(
    #                                   12000, 14500, 8226, 11207, blueminionpos),
    #                               minions_in_area(11134, 11207, 18000, 18000, blueminionpos))

    # if bot_wave_pushed == 'N/A':
    #     bot_wave_pushed = minionadv(red_bot_inner, blue_bot_inner)

    d = {
        "champs": [],
        "time": round(obj.time, 5),
        "index": snapshot_index,
        "jungle": jungle,
        "minions": minions,
        "others": others,
        "dragon": dragon,
        "herald": herald,
        "baron": baron,
        # "red_minions": redminionpos,
        # "blue_minions": blueminionpos,
        #  "red_minions_top_inner": red_top_inner,
        #  "blue_minions_top_inner": blue_top_inner,
        #  "red_minions_mid_inner": red_mid_inner,
        #  "blue_minions_mid_inner": blue_mid_inner,
        #  "red_minions_bot_inner": red_bot_inner,
        #  "blue_minions_bot_inner": blue_bot_inner,
        #  "blue_side_top_l2_blue_minions": minions_in_area(900, 2500, 6699, 10350, blueminionpos),
        #  "blue_side_top_l2_red_minions": minions_in_area(900, 2500, 6699, 10350, redminionpos),
        #  "blue_side_top_l3_blue_minions": minions_in_area(900, 2500, 4252, 6699, blueminionpos),
        #  "blue_side_top_l3_red_minions": minions_in_area(900, 2500, 4252, 6699, redminionpos),
        #  "blue_side_mid_l2_blue_minions": minions_in_area(5048, 5800, 4812, 6200, blueminionpos),
        #  "blue_side_mid_l2_red_minions": minions_in_area(5048, 5800, 4812, 6200, redminionpos),
        #  "blue_side_mid_l3_blue_minions": minions_in_area(3620, 5048, 3696, 4812, blueminionpos),
        #  "blue_side_mid_l3_red_minions": minions_in_area(3651, 5048, 3696, 4812, redminionpos),
        #  "blue_side_bot_l2_blue_minions": minions_in_area(6919, 10400, 900, 2500, blueminionpos),
        #  "blue_side_bot_l2_red_minions": minions_in_area(6919, 10400, 900, 2500, redminionpos),
        #  "blue_side_bot_l3_blue_minions": minions_in_area(4281, 6919, 900, 2500, blueminionpos),
        #  "blue_side_bot_l3_red_minions": minions_in_area(4281, 6919, 900, 2500, redminionpos),
        #  "blue_side_base_blue_minions": minions_in_area(0, 3651, 0, 3696, blueminionpos),
        #  "blue_side_base_red_minions": minions_in_area(0, 3651, 0, 3696, redminionpos),
        #  "red_side_top_l2_blue_minions": minions_in_area(4550, 7952, 12500, 14000, blueminionpos),
        #  "red_side_top_l2_red_minions": minions_in_area(4550, 7952, 12500, 14000, redminionpos),
        #  "red_side_top_l3_blue_minions": minions_in_area(7952, 10481, 12500, 14000, blueminionpos),
        #  "red_side_top_l3_red_minions": minions_in_area(7952, 10481, 12500, 14000, redminionpos),
        #  "red_side_mid_l2_blue_minions": minions_in_area(8955, 9767, 8510, 10113, blueminionpos),
        #  "red_side_mid_l2_red_minions": minions_in_area(8955, 9767, 8510, 10113, redminionpos),
        #  "red_side_mid_l3_blue_minions": minions_in_area(9767, 11134, 10113, 11207, blueminionpos),
        #  "red_side_mid_l3_red_minions": minions_in_area(9767, 11134, 10113, 11207, redminionpos),
        #  "red_side_bot_l2_blue_minions": minions_in_area(12000, 14500, 4505, 8226, blueminionpos),
        #  "red_side_bot_l2_red_minions": minions_in_area(12000, 14500, 4505, 8226, redminionpos),
        #  "red_side_bot_l3_blue_minions": minions_in_area(12000, 14500, 8226, 11207, blueminionpos),
        #  "red_side_bot_l3_red_minions": minions_in_area(12000, 14500, 8226, 11207, redminionpos),
        #  "red_side_base_blue_minions": minions_in_area(11134, 11207, 18000, 18000, blueminionpos),
        #  "red_side_base_red_minions": minions_in_area(11134, 11207, 18000, 18000, redminionpos),
        #  "minion_advantage_top_inner": minionadv(red_top_inner, blue_top_inner),
        #  "minion_advantage_mid_inner": minionadv(red_mid_inner, blue_mid_inner),
        #  "minion_advantage_bot_inner": minionadv(red_bot_inner, blue_bot_inner),
        #  "top_lane_pushed": top_wave_pushed,
        #  "mid_lane_pushed": mid_wave_pushed,
        #  "bot_lane_pushed": bot_wave_pushed
    }
    # print("Current Time:", gametime)
    print("dump")
    for champ in champlist:
        d['champs'].append({
            "champion": champ.name,
            "x": round(champ.pos.x, 4),
            "y": round(champ.pos.y, 4),
            "z": round(champ.pos.z, 4),
            # "items": [],
            "team": team[champ.team],
            "hp": round(champ.health, 4),
            "max_hp": round(champ.max_health, 4),
            "is_alive": champ.is_alive,
            "is_visible": champ.is_visible,
            "is_recalling": champ.isRecalling,
            "bonus_atk": round(champ.bonus_atk, 4),
            "armor": round(champ.armour, 4),
            "mr": round(champ.magic_resist, 4),
            "ap": champ.ap,
            "crit": champ.crit,
            # "crit_multi": champ.crit_multi,
            "atk_range": champ.atk_range,
            "atk_speed_multi": round(champ.atk_speed_multi, 4),
            "movement_speed": champ.movement_speed,
            "Q": {
                "level": champ.Q.level,
                "ready_at": champ.Q.ready_at,
                "width": champ.Q.width,
                "delay": champ.Q.delay
            },
            "W": {
                "level": champ.W.level,
                "ready_at": champ.W.ready_at,
                "width": champ.W.width,
                "delay": champ.W.delay
            },
            "E": {
                "level": champ.E.level,
                "ready_at": champ.E.ready_at,
                "width": champ.E.width,
                "delay": champ.E.delay
            },
            "R": {
                "level": champ.R.level,
                "ready_at": champ.R.ready_at,
                "width": champ.R.width,
                "delay": champ.R.delay
            },
            "D": {
                "name": champ.D.summoner_spell_type,
                "ready_at": champ.D.ready_at
            },
            "F": {
                "name": champ.F.summoner_spell_type,
                "ready_at": champ.F.ready_at
            }
        })
    # if obj.time == gametime and gametime > 20:
    #     results = list(db.temp.find())
    #     print("First")
    #     db['13.8'].update_one(db['13.8'].find_one(sort=[(
    #         "index", -1), ("page_index", -1)], limit=1), {"$push": {"snapshots": {"$each": results}}})
    #     db.temp.delete_many({})
    #     os.system(r'taskkill /im "League of Legends.exe" /f')
    # elif snapshot_index % 50 == 0:
    # print("second")
    # results = list(db.temp.find())
    # print(results)
    # db['13.8'].update_one(db['13.8'].find_one(sort=[(
    #     "index", -1), ("page_index", -1)], limit=1), {"$push": {"snapshots": {"$each": results}}})
    # page_index += 1
    # db['13.8'].insert_one({"index": db['13.8'].find_one(sort=[(
    #     "index", -1), ("page_index", -1)], limit=1)['index'], "page_index": page_index, "snapshots": []})
    # db.temp.delete_many({})
    # else:
    # db.temp.insert_one(d)
    snapshot_index += 1


def LViewPlus64_update(game, ui):
    try:
        dump(game)
    except Exception as e:
        traceback.print_exc()
        exit()
