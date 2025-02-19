USE [game]
GO

SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[ban_account](
	[id] [bigint] IDENTITY(1,1) NOT NULL,
	[account_id] [bigint] NOT NULL,
	[start_date] [datetime2](0) NOT NULL,
	[end_date] [datetime2](0) NOT NULL,
	[reason] [nvarchar](max) NOT NULL,
 CONSTRAINT [PK_ban_account_id] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO

ALTER TABLE [dbo].[ban_account]  WITH CHECK ADD  CONSTRAINT [FK_account_id] FOREIGN KEY([account_id])
REFERENCES [dbo].[account] ([id])
ON DELETE CASCADE
GO

ALTER TABLE [dbo].[ban_account] CHECK CONSTRAINT [FK_account_id]
GO
