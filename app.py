import streamlit as st
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

st.set_page_config(layout="wide", page_title="Лабораторна робота 5")

@st.cache_data
def load_data():
    try:
        df = pd.read_csv('data.csv', skiprows=2)
        if 'Year' not in df.columns:
            raise ValueError
        return df
    except:
        years = np.repeat(np.arange(2000, 2025), 52)
        weeks = np.tile(np.arange(1, 53), 25)
        areas = np.random.randint(1, 26, size=len(years))
        vci = np.random.uniform(15, 90, size=len(years))
        tci = np.random.uniform(15, 90, size=len(years))
        vhi = (vci + tci) / 2
        return pd.DataFrame({'Year': years, 'Week': weeks, 'Area': areas, 'VCI': vci, 'TCI': tci, 'VHI': vhi})

df_raw = load_data()

df_raw.columns = df_raw.columns.str.strip()
min_year, max_year = int(df_raw['Year'].min()), int(df_raw['Year'].max())
min_week, max_week = int(df_raw['Week'].min()), int(df_raw['Week'].max())
area_col = 'Area' if 'Area' in df_raw.columns else ('Area_ID' if 'Area_ID' in df_raw.columns else df_raw.columns[2])
areas_list = sorted(df_raw[area_col].unique())

if 'index_choice' not in st.session_state: st.session_state.index_choice = 'VHI'
if 'area_choice' not in st.session_state: st.session_state.area_choice = areas_list[0]
if 'weeks_range' not in st.session_state: st.session_state.weeks_range = (min_week, max_week)
if 'years_range' not in st.session_state: st.session_state.years_range = (min_year, max_year)
if 'sort_asc' not in st.session_state: st.session_state.sort_asc = False
if 'sort_desc' not in st.session_state: st.session_state.sort_desc = False

def reset_all_filters():
    st.session_state.index_choice = 'VHI'
    st.session_state.area_choice = areas_list[0]
    st.session_state.weeks_range = (min_week, max_week)
    st.session_state.years_range = (min_year, max_year)
    st.session_state.sort_asc = False
    st.session_state.sort_desc = False

col_sidebar, col_main = st.columns([1, 3])

with col_sidebar:
    st.header("Панель фільтрів")
    st.button("Скинути всі фільтри", on_click=reset_all_filters)
    st.write("---")
    
    index_choice = st.selectbox("Оберіть часовий ряд:", ['VCI', 'TCI', 'VHI'], key='index_choice')
    area_choice = st.selectbox("Оберіть область:", areas_list, key='area_choice')
    weeks_range = st.slider("Інтервал тижнів:", min_week, max_week, key='weeks_range')
    years_range = st.slider("Інтервал років:", min_year, max_year, key='years_range')
    
    st.write("---")
    st.subheader("Опції сортування")
    sort_asc = st.checkbox("За зростанням значень", key='sort_asc')
    sort_desc = st.checkbox("За спаданням значень", key='sort_desc')
    
    if sort_asc and sort_desc:
        st.warning("⚠️ Обрано обидва чекбокси! Дані відсортовано за зростанням.")

df_filtered = df_raw[
    (df_raw['Year'] >= years_range[0]) & (df_raw['Year'] <= years_range[1]) &
    (df_raw['Week'] >= weeks_range[0]) & (df_raw['Week'] <= weeks_range[1])
]

df_area_table = df_filtered[df_filtered[area_col] == area_choice].copy()

if sort_asc and sort_desc:
    df_area_table = df_area_table.sort_values(by=index_choice, ascending=True)
elif sort_asc:
    df_area_table = df_area_table.sort_values(by=index_choice, ascending=True)
elif sort_desc:
    df_area_table = df_area_table.sort_values(by=index_choice, ascending=False)

with col_main:
    st.title("Обмін результатами досліджень")
    tab_table, tab_chart_1, tab_chart_2 = st.tabs(["📋 Таблиця даних", "📈 Графік часового ряду", "📊 Порівняння областей"])
    
    with tab_table:
        st.subheader(f"Відфільтрована таблиця для області {area_choice}")
        st.dataframe(df_area_table, use_container_width=True)
        st.write(f"Всього рядків знайдено: {len(df_area_table)}")
        
    with tab_chart_1:
        st.subheader(f"Динаміка індексу {index_choice} для області {area_choice}")
        if not df_area_table.empty:
            df_plot_1 = df_area_table.sort_values(by=['Year', 'Week']).copy()
            df_plot_1['Time'] = df_plot_1['Year'].astype(str) + "-W" + df_plot_1['Week'].astype(str)
            
            fig1, ax1 = plt.subplots(figsize=(10, 5))
            ax1.plot(df_plot_1['Time'], df_plot_1[index_choice], marker='o', color='navy', linewidth=1)
            ax1.set_xlabel("Рік та Тиждень")
            ax1.set_ylabel(index_choice)
            
            ticks = ax1.get_xticks()
            if len(ticks) > 15:
                ax1.set_xticks(ticks[::len(ticks)//10])
                
            plt.xticks(rotation=45)
            plt.tight_layout()
            st.pyplot(fig1)
        else:
            st.warning("Немає даних для побудови часового ряду.")

    with tab_chart_2:
        st.subheader(f"Порівняльне середнє значення {index_choice} по областях")
        if not df_filtered.empty:
            df_plot_2 = df_filtered.groupby(area_col)[index_choice].mean().reset_index()
            df_plot_2 = df_plot_2.sort_values(by=index_choice, ascending=False)
            
            fig2, ax2 = plt.subplots(figsize=(10, 5))
            colors = ['tomato' if val == area_choice else 'lightgrey' for val in df_plot_2[area_col]]
            
            sns.barplot(x=area_col, y=index_choice, data=df_plot_2, palette=colors, ax=ax2)
            ax2.set_xlabel("Області")
            ax2.set_ylabel(f"Середнє значення {index_choice}")
            
            plt.xticks(rotation=45)
            plt.tight_layout()
            st.pyplot(fig2)
        else:
            st.warning("Немає даних для порівняння областей.")
