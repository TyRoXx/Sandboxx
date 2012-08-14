#include <gtkmm.h>
using namespace Gtk;

struct EditorWindow : Gtk::Window
{
	EditorWindow()
		: m_actions(Gtk::ActionGroup::create())
		, m_vbox(false)
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
		m_vbox.pack_start(m_status, Gtk::PACK_SHRINK);
		add(m_vbox);

		show_all();

		Glib::RefPtr<TextBuffer> buffer(TextBuffer::create(TextTagTable::create()));
		m_text.set_buffer(buffer);
	}

private:

	Glib::RefPtr<Gtk::ActionGroup> m_actions;
	Gtk::VBox m_vbox;
	Gtk::ScrolledWindow m_textScroll;
	Gtk::TextView m_text;
	Gtk::Statusbar m_status;


	void on_action_file_new()
	{

	}

	void on_action_file_open()
	{
		Gtk::FileChooserDialog dialog(*this, "Open file", Gtk::FILE_CHOOSER_ACTION_OPEN);
		dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
		dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
		dialog.set_select_multiple(true);
		const int result = dialog.run();

		switch (result)
		{
		case Gtk::RESPONSE_OK:
			{
				const auto fileNames = dialog.get_filenames();
				break;
			}

		default:
			break;
		}
	}

	void on_action_file_save()
	{
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
