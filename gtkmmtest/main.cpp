#include <gtkmm.h>
#include <iostream>
#include <fstream>
#include <vector>
using namespace Gtk;


struct EditorWindow : Gtk::Window
{
	EditorWindow()
		: m_actions(Gtk::ActionGroup::create())
		, m_vbox(false)
		, m_hasUnsavedChanges(false)
	{
		set_default_size(600, 400);
		set_icon_from_file("faudra.ico");

		m_actions->add(Gtk::Action::create("MenuFile", "_File"));
		m_actions->add(Gtk::Action::create("New", Gtk::Stock::NEW), sigc::mem_fun(*this, &EditorWindow::on_action_file_new));
		m_actions->add(Gtk::Action::create("Open", Gtk::Stock::OPEN), sigc::mem_fun(*this, &EditorWindow::on_action_file_open));
		m_actions->add(Gtk::Action::create("Save", Gtk::Stock::SAVE), sigc::mem_fun(*this, &EditorWindow::on_action_file_save));
		m_actions->add(Gtk::Action::create("Save as", Gtk::Stock::SAVE_AS), sigc::mem_fun(*this, &EditorWindow::on_action_file_save_as));

		const auto saveAll = Gtk::Action::create("Save all", "Save _all");
		
		m_actions->add(saveAll, sigc::mem_fun(*this, &EditorWindow::on_action_file_save_all));
		m_actions->add(Gtk::Action::create("Quit", Gtk::Stock::QUIT), sigc::mem_fun(*this, &EditorWindow::on_action_file_quit));

		Glib::RefPtr<Gtk::UIManager> UIManager = Gtk::UIManager::create();
		UIManager->insert_action_group(m_actions);
		add_accel_group(UIManager->get_accel_group());

		Glib::ustring ui_info =
			"<ui>"
			"  <menubar name='MenuBar'>"
			"    <menu action='MenuFile'>"
			"      <menuitem action='New'/>"
			"      <menuitem action='Open'/>"
			"      <menuitem action='Save'/>"
			"      <menuitem action='Save as'/>"
			"      <menuitem action='Save all'/>"
			"      <separator/>"
			"      <menuitem action='Quit'/>"
			"    </menu>"
			"  </menubar>"
			"  <toolbar name='ToolBar'>"
			"    <toolitem action='Open'/>"
			"    <toolitem action='Save'/>"
			"    <toolitem action='Save as'/>"
			"    <toolitem action='Save all'/>"
			"  </toolbar>"
			"</ui>";

		UIManager->add_ui_from_string(ui_info);

		m_textScroll.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		m_textScroll.add(m_text);

		Gtk::Widget* pMenuBar = UIManager->get_widget("/MenuBar");
		Gtk::Widget* pToolbar = UIManager->get_widget("/ToolBar");
		
		m_vbox.pack_start(*pMenuBar, Gtk::PACK_SHRINK);
		m_vbox.pack_start(*pToolbar, Gtk::PACK_SHRINK);
		m_vbox.pack_start(m_textScroll);
		add(m_vbox);

		show_all();

		m_textBuffer = TextBuffer::create(TextTagTable::create());
		m_text.set_buffer(m_textBuffer);

		m_text.modify_font(Pango::FontDescription("Consolas"));
	}

private:

	Glib::RefPtr<Gtk::ActionGroup> m_actions;
	Gtk::VBox m_vbox;
	Gtk::ScrolledWindow m_textScroll;
	Gtk::TextView m_text;
	Glib::RefPtr<TextBuffer> m_textBuffer;
	bool m_hasUnsavedChanges;
	Glib::ustring m_currentFileName;


	bool load_file(const Glib::ustring &fileName)
	{
		std::ifstream file(fileName, std::ios::binary);
		if (!file)
		{
			return false;
		}

		std::vector<char> content(
			(std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());

		m_textBuffer->set_text(
			content.data(),
			content.data() + content.size());
		return true;
	}

	bool save_file(const Glib::ustring &fileName)
	{
		std::ofstream file(fileName, std::ios::binary);
		if (!file)
		{
			return false;
		}

		const Glib::ustring content = m_textBuffer->get_text();
		file.write(content.data(), content.size());
		return true;
	}

	void on_action_file_new()
	{
		if (m_hasUnsavedChanges)
		{
			Gtk::MessageDialog dialog(*this, "Discard unsaved changes?", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO, true);
			const int result = dialog.run();
			switch (result)
			{
			case Gtk::RESPONSE_YES:
				break;

			default:
				return;
			}
		}

		m_textBuffer->set_text(Glib::ustring());
		m_hasUnsavedChanges = false;
		m_currentFileName.clear();
	}

	void on_action_file_open()
	{
		Gtk::FileChooserDialog dialog(*this, "Open file", Gtk::FILE_CHOOSER_ACTION_OPEN);
		dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
		dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
		dialog.set_select_multiple(false);
		const int result = dialog.run();

		switch (result)
		{
		case Gtk::RESPONSE_OK:
			{
				const auto fileName = dialog.get_filename();
				if (load_file(fileName))
				{
					m_hasUnsavedChanges = false;
					m_currentFileName = fileName;
				}
				else
				{
					//TODO
				}
				break;
			}

		default:
			break;
		}
	}

	void on_action_file_save()
	{
		Glib::ustring fileName;

		if (m_currentFileName.empty())
		{
			Gtk::FileChooserDialog dialog(*this, "Choose a file name", Gtk::FILE_CHOOSER_ACTION_SAVE);
			dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
			dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
			dialog.set_select_multiple(false);
			const int result = dialog.run();

			switch (result)
			{
			case Gtk::RESPONSE_OK:
				{
					fileName = dialog.get_filename();
					break;
				}

			default:
				return;
			}
		}
		else
		{
			fileName = m_currentFileName;
		}

		if (save_file(fileName))
		{
			m_hasUnsavedChanges = false;
			m_currentFileName = fileName;
		}
		else
		{
			//TODO
		}
	}

	void on_action_file_save_as()
	{
	}

	void on_action_file_save_all()
	{
	}

	void on_action_file_quit()
	{
		hide();
	}
};

int main(int argc, char *argv[])
{
	Gtk::Main main_obj(argc, argv);
	EditorWindow window_obj;
	main_obj.run(window_obj);
	return 0;
}
